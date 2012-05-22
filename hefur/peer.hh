#ifndef HEFUR_PEER_HH
# define HEFUR_PEER_HH

# include <cstdint>

# include <mimosa/intrusive-dlist.hh>

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
    uint32_t      timeout_ts_; // monotonic coarse clock

    mimosa::IntrusiveDListHook<Peer*> timeout_hook_;

    static mimosa::StringRef key(Peer * peer) { return peer->addr_.key(); }
  };
}

#endif /* !HEFUR_PEER_HH */
