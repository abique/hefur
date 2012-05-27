#include <mimosa/time.hh>

#include "options.hh"
#include "torrent.hh"

namespace hefur
{
  Torrent::Torrent(const InfoSha1 &    info_sha1,
                   const std::string & name,
                   const std::string & path)
    : info_sha1_(info_sha1),
      name_(name),
      path_(path),
      timeouts_(),
      peers_(),
      leechers_(0),
      seeders_(0),
      completed_(0),
      downloaded_(0),
      uploaded_(0)
  {
  }

  Torrent::~Torrent()
  {
    while (!timeouts_.empty())
    {
      auto peer = timeouts_.front();
      timeouts_.erase(peer);
      delete peer;
    }
  }

  void
  Torrent::cleanup()
  {
    mimosa::Time now = mimosa::monotonicTimeCoarse();

    while (!timeouts_.empty())
    {
      auto peer = timeouts_.front();
      if (peer->timeout_ts_ < now)
        return;

      removePeer(peer);
      timeouts_.erase(peer);
      peers_.erase(Peer::key(peer));
      delete peer;
    }
  }

  AnnounceResponse::Ptr
  Torrent::announce(AnnounceRequest::Ptr request)
  {
    // prepare the response
    AnnounceResponse::Ptr response = new AnnounceResponse;
    response->error_              = false;
    response->error_msg_.clear();
    response->nleechers_          = leechers_;
    response->nseeders_           = seeders_;
    response->ncompleted_         = completed_;

    // update stats
    downloaded_ += request->downloaded_;
    uploaded_   += request->uploaded_;

    // find the peer
    Peer * peer = peers_.find(mimosa::StringRef((const char*)request->peerid_ + 8, 12));

    // update counters to reflect a leecher becoming seeder
    if (peer && peer->left_ > 0 && request->left_ == 0 &&
        request->event_ == AnnounceRequest::kCompleted)
    {
      --leechers_;
      ++seeders_;
    }

    // the peer wants to stop, so remove it from the peers
    if (request->event_ == AnnounceRequest::kStopped)
    {
      if (!peer)
      {
        response->error_ = true;
        response->error_msg_ = "peer not found";
        return response;
      }

      removePeer(peer);
      return response;
    }

    // create the peer
    if (!peer)
    {
      peer = createPeer(request);
      if (!peer)
      {
        response->error_ = true;
        response->error_msg_ = "internal error (2)";
        return response;
      }
    }

    if (request->num_want_ > 100)
      request->num_want_ = 100;

    for (auto it = timeouts_.rbegin(); it != timeouts_.rend(); ++it)
    {
      if (response->addrs_.size() >= request->num_want_)
        break;

      if (!::memcmp(it->peerid_, request->peerid_, 20))
        continue;

      response->addrs_.push_back(it->addr_);
    }

    return response;
  }

  Peer *
  Torrent::createPeer(AnnounceRequest::Ptr request)
  {
    Peer * peer       = new Peer;
    peer->addr_       = request->addr_;
    peer->left_       = request->left_;
    peer->downloaded_ = request->downloaded_;
    peer->uploaded_   = request->uploaded_;
    peer->timeout_ts_ = mimosa::monotonicTimeCoarse() +
      mimosa::minute * ANNOUNCE_INTERVAL * 3 / 2;
    memcpy(peer->peerid_, request->peerid_, 20);

    if (peer->left_ == 0)
      ++seeders_;
    else
      ++leechers_;
    peers_.insert(peer);
    timeouts_.pushBack(peer);
    return peer;
  }

  void
  Torrent::removePeer(Peer * peer)
  {
    if (peer->left_ == 0)
      --seeders_;
    else
      --leechers_;

    timeouts_.erase(peer);
    peers_.erase(peer);
    delete peer;
  }
}
