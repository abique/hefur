#pragma once

#include <string>
#include <functional>
#include <unordered_set>

#include <mimosa/future.hh>
#include <mimosa/shared-mutex.hh>
#include <mimosa/thread.hh>
#include <mimosa/trie.hh>

#include "namespace-helper.hh"
#include "info-hash.hh"
#include "torrent.hh"

namespace hefur
{
  /**
   * This class periodically scans root_ to find torrents, and
   * add them to the torrent db. It has it owns thread.
   */
  class FsTreeWhiteList
  {
  public:
    explicit FsTreeWhiteList(const std::string & root, m::Time rescan_interval = m::minute);
    ~FsTreeWhiteList();

    /**
     * Scans root_, can be called at anytime, from any thread.
     * When a .torrent is found, call TorrentDb::addTorrent().
     */
    void scan();
    void check();

  private:
    /**
     * @internal Ran into a dedicated thread, started in the constructor.
     * Will run until stop_ is set to true.
     */
    void loopScan();
    void checkTorrent(Torrent::Ptr torrent, std::vector<m::StringRef> &keys) const;

    const std::string & root_;
    m::Time             rescan_interval_;
    m::Future<bool>     stop_;
    m::Thread         scan_thread_;
  };
}
