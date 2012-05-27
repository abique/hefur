#include <fnmatch.h>

#include <mimosa/fs/find.hh>

#include "hefur.hh"
#include "fs-tree-white-list.hh"

namespace hefur
{
  FsTreeWhiteList::FsTreeWhiteList(const std::string & root,
                                   uint32_t            rescan_period)
    : root_(root),
      rescan_period_(rescan_period),
      stop_(),
      scan_thread_([this] { this->loopScan(); })
  {
    scan_thread_.start();
  }

  FsTreeWhiteList::~FsTreeWhiteList()
  {
    stop_.set(true);
    scan_thread_.join();
  }

  void
  FsTreeWhiteList::scan()
  {
    mimosa::fs::find(root_, true, [&] (const std::string & path) {
        if (::fnmatch("*.torrent", path.c_str(), FNM_CASEFOLD))
          return;

        Hefur::instance().torrentDb().addTorrent(path);
      });
  }

  void
  FsTreeWhiteList::loopScan()
  {
    do
      scan();
    while (!stop_.timedWait(mimosa::time() + mimosa::second * rescan_period_));
  }
}
