#include <fnmatch.h>

#include <mimosa/bittorrent/torrent-parser.hh>
#include <mimosa/fs/find.hh>

#include "fs-tree-white-list.hh"
#include "hefur.hh"
#include "info-hash.hh"
#include "log.hh"
#include "options.hh"

#ifndef FNM_CASEFOLD
#   define FNM_CASEFOLD 0
#endif

#ifndef FNM_IGNORECASE
#   define FNM_IGNORECASE 0
#endif

namespace hefur {
   FsTreeWhiteList::FsTreeWhiteList(const std::string &root, m::Time rescan_interval)
      : root_(root), rescan_interval_(rescan_interval), stop_(),
        scan_thread_([this] { this->loopScan(); }) {}

   FsTreeWhiteList::~FsTreeWhiteList() {
      stop_.set(true);
      scan_thread_.join();
   }

   void FsTreeWhiteList::loopScan() {
      do {
         scan();
         check();
      } while (!stop_.timedWait(m::time() + rescan_interval_));
   }

   void FsTreeWhiteList::scan() {
      std::unordered_set<InfoHash> info_hashes;
      uint32_t nb_inodes = 0;

      m::fs::find(root_, MAX_SCAN_DEPTH, [&](const std::string &path) {
         if (++nb_inodes > MAX_SCAN_INODES) {
            log->error("reached the limit of scanned inodes: %d", MAX_SCAN_INODES);
            return false;
         }

         if (::fnmatch("*.torrent", path.c_str(), FNM_CASEFOLD | FNM_IGNORECASE))
            return true;

         auto tdb = Hefur::instance().torrentDb();
         if (tdb) {
            m::bittorrent::TorrentParser p;

            if (!p.parseFile(path)) {
               log->error("%s: parse error", path);
               return true;
            }

            auto desc = p.result();

            InfoHash info_v1(InfoHash::SHA1, reinterpret_cast<const char *>(desc->info_hash_v1_.bytes_));
            tdb->addTorrent(new Torrent(info_v1, desc->name_, path, desc->length_));

            InfoHash info_v2(InfoHash::SHA256, reinterpret_cast<const char *>(desc->info_hash_v2_.bytes_));
            tdb->addTorrent(new Torrent(info_v2, desc->name_, path, desc->length_));
         }
         return true;
      });
   }

   void FsTreeWhiteList::check() {
      std::vector<std::string_view> keys;
      auto db = Hefur::instance().torrentDb();
      m::SharedMutex::Locker locker(db->torrents_lock_);
      db->torrents_.foreach (
         [this, &keys](Torrent::Ptr torrent) { checkTorrent(torrent, keys); });

      for (auto it = keys.begin(); it != keys.end(); ++it)
         db->torrents_.erase(*it);
   }

   void FsTreeWhiteList::checkTorrent(Torrent::Ptr torrent,
                                      std::vector<std::string_view> &keys) const {
      if (::strncmp(torrent->path().c_str(), root_.c_str(), root_.size()))
         return;

      struct ::stat st;
      if (::stat(torrent->path().c_str(), &st) && errno == ENOENT) {
         keys.emplace_back(torrent->key());
      }
   }
} // namespace hefur
