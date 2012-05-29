#ifndef HEFUR_PEER_HH
# define HEFUR_PEER_HH

# include <cstdint>

# include <mimosa/intrusive-dlist.hh>
# include <mimosa/time.hh>

# include "address.hh"
# include "crypto-support.hh"

namespace hefur
{
  class Peer
  {
  public:
    Peer();

    Address       addr_;
    uint64_t      left_;
    uint64_t      downloaded_;
    uint64_t      uploaded_;
    mimosa::Time  timeout_ts_; // monotonic coarse clock
    uint8_t       peerid_[20];

    mimosa::IntrusiveDListHook<Peer*> timeout_hook_;

    static inline mimosa::StringRef key(Peer * peer) {
      // don't use the 8 first bytes as they're not really relevant:
      // they represent the peer's client and version.
      return mimosa::StringRef((const char *)peer->peerid_ + 8, 12);
    }
  };
}

#endif /* !HEFUR_PEER_HH */
