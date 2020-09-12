#pragma once

#include <string>

#include <mimosa/intrusive-dlist.hh>
#include <mimosa/mutex.hh>
#include <mimosa/ref-countable.hh>
#include <mimosa/trie.hh>
#include <string_view>

#include "address.hh"
#include "announce-request.hh"
#include "announce-response.hh"
#include "info-hash.hh"
#include "peer.hh"

namespace hefur {
   class PeersHandler;

   class Torrent final : public m::RefCountable<Torrent> {
   public:
      Torrent(const InfoHash &info_hash,
              const std::string &name = "", const std::string &path = "", uint64_t length = 0);
      ~Torrent();

      /**
       * Handle an announce request and returns an announce response.
       * Thread safe.
       *
       * The return value must never be null.
       */
      AnnounceResponse::Ptr announce(AnnounceRequest::Ptr request);

      /**
       * Removes timed-out peers. Thread safe.
       */
      void cleanup();

      /**
       * Return a key of the info hash to use in a Trie.
       */
      inline std::string_view key() const noexcept { return info_hash_.bytes(); }

      inline const std::string &name() const { return name_; }
      inline const std::string &path() const { return path_; }
      inline uint64_t length() const { return length_; }
      inline uint32_t leechers() const { return leechers_; }
      inline uint32_t seeders() const { return seeders_; }
      inline uint32_t completed() const { return completed_; }

   private:
      friend class PeersHandler;

      /**
       * Create a peer from an announce request and add the peer
       * to peers_ and timeouts_.
       */
      Peer *createOrUpdatePeer(AnnounceRequest::Ptr request, Peer *peer = nullptr);

      /**
       * Updates peer's timeout and place it at the back of timeouts_.
       *
       * @param remove if true, remove the peer from timeouts_ before placing
       * it at the back.
       */
      void updateTimeout(Peer *peer, bool remove);

      /**
       * Remove the peer from peers_ and timeouts_ and update stats.
       */
      void removePeer(Peer *peer);

      using timeouts_type = m::IntrusiveDList<Peer, Peer *, &Peer::timeout_hook_>;
      using peers_type = m::Trie<Peer *, Peer::addr>;

      m::Mutex lock_;
      InfoHash info_hash_; // this is the torrent key
      std::string name_;      // optional, used by StatHandler
      std::string path_;      // optional, for later download
      uint64_t length_;
      timeouts_type timeouts_; // this is a timeout sorted list of peers
      peers_type peers_;       // this is an index of peers by using their peer id.

      // stats
      uint32_t leechers_;  // the number of leechers
      uint32_t seeders_;   // the number of seeders
      uint32_t completed_; // the number of times a peer completed the download
   };

} // namespace hefur
