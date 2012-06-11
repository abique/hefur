#ifndef HEFUR_FS_TREE_WHITE_LIST_HH
# define HEFUR_FS_TREE_WHITE_LIST_HH

# include <string>
# include <functional>
# include <unordered_set>

# include <mimosa/future.hh>
# include <mimosa/shared-mutex.hh>
# include <mimosa/thread.hh>
# include <mimosa/trie.hh>

# include "info-sha1.hh"
# include "white-list.hh"

namespace hefur
{
  /**
   * This class periodically scans root_ to find torrents, and
   * add them to the torrent db. It has it owns thread.
   */
  class FsTreeWhiteList
  {
  public:
    FsTreeWhiteList(const std::string & root,
                    m::Time             rescan_interval = m::minute);
    ~FsTreeWhiteList();

    /**
     * Scans root_, can be called at anytime, from any thread.
     * When a .torrent is found, call TorrentDb::addTorrent().
     */
    void scan();

  private:
    /**
     * @internal Ran into a dedicated thread, started in the constructor.
     * Will run until stop_ is set to true.
     */
    void loopScan();

    const std::string & root_;
    m::Time             rescan_interval_;
    m::Future<bool>     stop_;
    m::Thread           scan_thread_;
  };
}

#endif /* !HEFUR_FS_TREE_WHITE_LIST_HH */
