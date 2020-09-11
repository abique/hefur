#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>

#include <mimosa/future.hh>
#include <mimosa/shared-mutex.hh>
#include <mimosa/thread.hh>
#include <mimosa/trie.hh>

#include "info-hash.hh"
#include "namespace-helper.hh"
#include "torrent.hh"

namespace hefur {
   /**
    * This class periodically scans root_ to find torrents, and
    * add them to the torrent db. It has it owns thread.
    */
   class FsTreeWhiteList {
   public:
      explicit FsTreeWhiteList(const std::string &root, m::Time rescan_interval = m::minute);
      ~FsTreeWhiteList();

      void lock() const { mutex_.lock(); }
      void unlock() const { mutex_.unlock(); }

   private:
      /**
       * Scans root_, can be called at anytime, from any thread.
       * When a .torrent is found, call TorrentDb::addTorrent().
       */
      void scan();
      void check();

      /**
       * @internal Ran into a dedicated thread, started in the constructor.
       * Will run until stop_ is set to true.
       */
      void loopScan();
      void checkTorrent(Torrent::Ptr torrent, std::vector<m::StringRef> &keys) const;

      mutable std::mutex mutex_;

      const std::string &root_;
      const m::Time rescan_interval_;
      m::Future<bool> stop_;
      std::thread scan_thread_;
      std::unordered_set<InfoHash> info_hashes_;
   };
} // namespace hefur
