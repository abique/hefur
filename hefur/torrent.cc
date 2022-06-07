#include <exception>
#include <mimosa/bencode/copy.hh>
#include <mimosa/bencode/decoder.hh>
#include <mimosa/bencode/encoder.hh>
#include <mimosa/bittorrent/torrent-parser.hh>
#include <mimosa/mutex.hh>
#include <mimosa/stream/fd-stream.hh>
#include <mimosa/stream/hash.hh>
#include <mimosa/stream/limited-stream.hh>
#include <mimosa/time.hh>
#include <mimosa/trie.hxx>

#include "address.hxx"
#include "announce-request.hh"
#include "info-hash.hh"
#include "log.hh"
#include "options.hh"
#include "torrent.hh"

namespace hefur {
   Torrent::Torrent(const InfoHash &info_hash, const std::string &name, const std::string &path,
                    uint64_t length)
      : info_hash_(info_hash), name_(name), path_(path), length_(length), timeouts_(), peers_(),
        leechers_(0), seeders_(0), completed_(0) {}

   Torrent::~Torrent() {
      m::Mutex::Locker locker(lock_);
      while (!timeouts_.empty()) {
         auto peer = timeouts_.front();
         timeouts_.erase(peer);
         delete peer;
      }
   }

   void Torrent::cleanup() {
      m::Mutex::Locker locker(lock_);
      m::Time now = m::monotonicTimeCoarse();

      while (!timeouts_.empty()) {
         auto peer = timeouts_.front();
         if (peer->timeout_ts_ > now)
            return;

         log->debug("remove peer (%v) because %v > %v", peer->addr_.ipStr(), now / m::second,
                    peer->timeout_ts_ / m::second);
         removePeer(peer);
      }
   }

   AnnounceResponse::Ptr Torrent::announce(AnnounceRequest::Ptr request) {
      m::Mutex::Locker locker(lock_);

      // prepare the response
      AnnounceResponse::Ptr response = new AnnounceResponse;
      response->error_ = false;
      response->error_msg_.clear();
      response->nleechers_ = leechers_;
      response->nseeders_ = seeders_;
      response->ncompleted_ = completed_;

      // find the peer
      Peer *peer = peers_.find(request->addr_.key());

      // update counters to reflect a leecher becoming seeder
      if (peer && peer->left_ > 0 && request->left_ == 0 &&
          request->event_ == AnnounceRequest::kCompleted)
         ++completed_;

      // the peer wants to stop, so remove it from the peers
      if (request->event_ == AnnounceRequest::kStopped) {
         if (peer)
            removePeer(peer);
         return response;
      }

      // create the peer
      if (!createOrUpdatePeer(request, peer)) {
         response->error_ = true;
         response->error_msg_ = "internal error (2)";
         return response;
      }

      // correct numwant option to intended borders
      request->num_want_ = std::clamp<int>(request->num_want_, MIN_NUMWANT, MAX_NUMWANT);

      for (auto it = timeouts_.rbegin(); it != timeouts_.rend(); ++it) {
         if (response->addrs_.size() >= request->num_want_)
            break;

         if (((request->addr_.family_ == AF_INET || request->skip_ipv6_) &&
              it->addr_.family_ == AF_INET6) ||
             !::memcmp(it->peerid_, request->peerid_, 20) || request->addr_ == it->addr_)
            continue;

         response->addrs_.push_back(it->addr_);
      }

      // check that we have less than MAX_PEERS
      if (leechers_ + seeders_ > MAX_PEERS)
         removePeer(timeouts_.front());

      return response;
   }

   Peer *Torrent::createOrUpdatePeer(AnnounceRequest::Ptr request, Peer *peer) {
      bool update = !!peer;

      // decrements the counters to simplify the completed event
      if (peer) {
         if (peer->left_ == 0)
            --seeders_;
         else
            --leechers_;
      }

      assert(seeders_ >= 0);
      assert(leechers_ >= 0);

      if (!peer)
         peer = new Peer;
      peer->left_ = request->left_;
      peer->downloaded_ = request->downloaded_;
      peer->uploaded_ = request->uploaded_;
      peer->addr_ = request->addr_;
      memcpy(peer->peerid_, request->peerid_, 20);

      // increments the counters
      if (peer->left_ == 0)
         ++seeders_;
      else
         ++leechers_;
      if (!update)
         peers_.insert(peer);
      updateTimeout(peer, update);
      return peer;
   }

   void Torrent::updateTimeout(Peer *peer, bool remove) {
      if (remove)
         timeouts_.erase(peer);
      peer->timeout_ts_ = m::monotonicTimeCoarse() + m::minute * (ANNOUNCE_INTERVAL + 5);
      timeouts_.pushBack(peer);
   }

   void Torrent::removePeer(Peer *peer) {
      if (peer->left_ == 0)
         --seeders_;
      else
         --leechers_;

      assert(seeders_ >= 0);
      assert(leechers_ >= 0);

      timeouts_.erase(peer);
      peers_.erase(peer);
      delete peer;
   }

   int Torrent::version() const noexcept {
      switch (info_hash_.type_) {
      case InfoHash::SHA1:
         return 1;
      case InfoHash::SHA256:
         return 2;
      default:
         std::terminate();
      }
   }
} // namespace hefur
