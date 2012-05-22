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
    return nullptr;
  }
}
