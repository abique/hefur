#pragma once

#include <cstdint>

#include <mimosa/intrusive-dlist.hh>
#include <mimosa/time.hh>

#include "address.hh"
#include "crypto-support.hh"
#include "namespace-helper.hh"

namespace hefur {
   /**
    * Represents a torrent's peer.
    */
   class Peer {
   public:
      Peer() = default;

      Address addr_;
      uint64_t left_ = 0;
      uint64_t downloaded_ = 0;
      uint64_t uploaded_ = 0;
      m::Time timeout_ts_ = 0; // monotonic coarse clock
      uint8_t peerid_[20] = {};

      m::IntrusiveDListHook<Peer *> timeout_hook_;

      static inline m::StringRef peerId(Peer *peer) {
         // don't use the 8 first bytes as they're not really relevant:
         // they represent the peer's client and version.
         return m::StringRef((const char *)peer->peerid_ + 8, 12);
      }

      static inline m::StringRef addr(Peer *peer) { return peer->addr_.key(); }
   };
} // namespace hefur
