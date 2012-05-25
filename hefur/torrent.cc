#include <mimosa/time.hh>

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
      peers_()
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
      if (peer->timeout_ts_ >= now)
        return;

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
    response->nleechers_          = nleechers_;
    response->nseeders_           = nseeders_;
    response->ncompleted_         = ncompleted_;

    // find the peer
    Peer * peer = peers_.find(mimosa::StringRef((const char*)request->peerid_ + 8, 12));

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
    }

    if (!peer && request->event_ != AnnounceRequest::kStarted)
    {
      peer = createPeer(request);
      if (!peer)
        return nullptr;
    }

    // XXX finish the announce
    return nullptr;
  }

  Peer *
  Torrent::createPeer(AnnounceRequest::Ptr request)
  {
    Peer * peer = new Peer;
    memcpy(peer->peerid_, request->peerid_, 20);
    peer->addr_ = request->addr_;
    peer->left_ = request->left_;
    peer->downloaded_ = request->downloaded_;
    peer->uploaded_ = request->uploaded_;
    peer->timeout_ts_ = mimosa::monotonicTimeCoarse() + mimosa::minute * 45;

    peers_.insert(peer);
    timeouts_.pushBack(peer);
    return peer;
  }

  void
  Torrent::removePeer(Peer * peer)
  {
    timeouts_.erase(peer);
    peers_.erase(peer);
    delete peer;
  }
}
