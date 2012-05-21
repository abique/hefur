#include <mimosa/time.hh>

#include "torrent.hh"

namespace hefur
{
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
      peers_.erase(peer);
    }
  }
}
