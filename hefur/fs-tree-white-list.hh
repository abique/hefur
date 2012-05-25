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
  class FsTreeWhiteList
  {
  public:
    FsTreeWhiteList(const std::string & root,
                    uint32_t            rescan_period = 60);
    ~FsTreeWhiteList();

    void scan();

  private:
    void loopScan();

    const std::string &  root_;
    uint32_t             rescan_period_;
    mimosa::Future<bool> stop_;
    mimosa::Thread       scan_thread_;
  };
}

#endif /* !HEFUR_FS_TREE_WHITE_LIST_HH */
