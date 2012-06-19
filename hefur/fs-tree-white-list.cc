#include <fnmatch.h>

#include <mimosa/fs/find.hh>

#include "fs-tree-white-list.hh"
#include "hefur.hh"
#include "options.hh"

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
    m::fs::find(root_, MAX_SCAN_DEPTH, [&] (const std::string & path) {
        if (::fnmatch("*.torrent", path.c_str(), FNM_CASEFOLD))
          return true;

        auto tdb = Hefur::instance().torrentDb();
        if (tdb)
          tdb->addTorrent(Torrent::parseFile(path));
        return true;
      });
  }

  void
  FsTreeWhiteList::loopScan()
  {
    do {
      scan();
    } while (!stop_.timedWait(m::time() + rescan_interval_));
  }
}
