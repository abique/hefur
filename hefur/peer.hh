#ifndef HEFUR_PEER_HH
# define HEFUR_PEER_HH

# include <cstdint>

# include <mimosa/container/intrusive-dlist.hh>

# include "address.hh"
# include "crypto-support.hh"

namespace hefur
{
  class Peer
  {
  public:
    Peer();

    char          peerid_[20];
    Address       addr_;
    uint16_t      crypto_port_;
    CryptoSupport crypto_support_;
    uint64_t      left_;
    uint64_t      downloaded_;
    uint64_t      uploaded_;
    uint32_t      timeout_ts_;

    mimosa::container::IntrusiveDListHook<Peer*> timeout_hook_;


    inline bool operator==(const Peer & other) const {
      return !memcmp(peerid_, other.peerid_, 20) &&
        addr_ == other.addr_;
    }

    inline bool operator<(const Peer & other) const {
      int ret = memcmp(peerid_, other.peerid_, 20);
      if (ret > 0)
        return false;
      if (ret < 0)
        return true;

      return addr_ < other.addr_;
    }

    // XXX improve me
    struct Hash
    {
      inline size_t operator()(const Peer * peer) const
      {
        return peer->addr_.in_.sin_addr.s_addr;
      }
    };

    struct Equal
    {
      inline bool operator()(const Peer * p1,
                             const Peer * p2) const
      {
        return *p1 == *p2;
      }
    };
  };
}

#endif /* !HEFUR_PEER_HH */
