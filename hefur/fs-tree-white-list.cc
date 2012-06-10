#include <fnmatch.h>

#include <mimosa/fs/find.hh>

#include "hefur.hh"
#include "fs-tree-white-list.hh"

namespace hefur
{
  FsTreeWhiteList::FsTreeWhiteList(const std::string & root,
                                   m::Time             rescan_interval)
    : root_(root),
      rescan_interval_(rescan_interval),
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

        Hefur::instance().torrentDb().addTorrent(Torrent::parseFile(path));
      });
  }

  void
  FsTreeWhiteList::loopScan()
  {
    do {
      scan();
    } while (!stop_.timedWait(mimosa::time() + rescan_interval_));
  }
}
