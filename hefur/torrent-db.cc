#include "torrent-db.hh"

namespace hefur
{
  TorrentDb::TorrentDb()
    : cleanup_stop_(),
      cleanup_thread_([this] { this->cleanupLoop(); }),
      torrents_()
  {
    cleanup_thread_.start();
  }

  TorrentDb::~TorrentDb()
  {
    cleanup_stop_.set(true);
    cleanup_thread_.join();
  }

  AnnounceResponse::Ptr
  TorrentDb::announce(AnnounceRequest::Ptr request)
  {
    mimosa::SharedMutex::Locker locker(torrents_lock_);
    return nullptr;
  }

  ScrapeResponse::Ptr
  TorrentDb::scrape(ScrapeRequest::Ptr request)
  {
    mimosa::SharedMutex::ReadLocker locker(torrents_lock_);
    return nullptr;
  }

  void
  TorrentDb::cleanup()
  {
    mimosa::SharedMutex::Locker locker(torrents_lock_);
    for (auto it = torrents_.begin(); it != torrents_.end(); ++it)
      const_cast<Torrent&> (*it).cleanup();
  }

  void
  TorrentDb::cleanupLoop()
  {
    while (!cleanup_stop_.timedWait(mimosa::time() + 5 * mimosa::minute))
      cleanup();
  }
}
