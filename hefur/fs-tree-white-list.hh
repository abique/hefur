#ifndef HEFUR_FS_TREE_WHITE_LIST_HH
# define HEFUR_FS_TREE_WHITE_LIST_HH

# include <string>
# include <functional>
# include <unordered_set>

# include <mimosa/future.hh>
# include <mimosa/shared-mutex.hh>
# include <mimosa/thread.hh>

# include "info-sha1.hh"
# include "white-list.hh"

namespace hefur
{
  class FsTreeWhiteList : public WhiteList
  {
  public:
    FsTreeWhiteList(const std::string & root,
                    uint32_t            rescan_period = 60);
    ~FsTreeWhiteList();

    virtual bool isAllowed(const InfoSha1 & info_sha1) const override;

    void scan();

  private:
    typedef std::unordered_set<InfoSha1, InfoSha1::BasicHasher> sha1s_type;

    void loadTorrent(sha1s_type & sha1s, const std::string & path);
    void loopScan();

    const std::string &         root_;
    uint32_t                    rescan_period_;
    mimosa::Future<bool>        stop_;
    sha1s_type                  sha1s_;
    mutable mimosa::SharedMutex sha1s_lock_;
    mimosa::Thread              scan_thread_;
  };
}

#endif /* !HEFUR_FS_TREE_WHITE_LIST_HH */
