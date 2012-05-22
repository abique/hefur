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
    mimosa::SharedMutex::Locker locker(torrents_lock_);
    cleanup_stop_.set(true);
    cleanup_thread_.join();
  }

  AnnounceResponse::Ptr
  TorrentDb::announce(AnnounceRequest::Ptr request)
  {
    mimosa::SharedMutex::Locker locker(torrents_lock_);

    Torrent * torrent = torrents_.find(request->info_sha1_.bytes());
    if (torrent)
      return torrent->announce(request);
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
    torrents_.foreach([] (Torrent::Ptr torrent) {
        torrent->cleanup();
      });
  }

  void
  TorrentDb::cleanupLoop()
  {
    while (!cleanup_stop_.timedWait(mimosa::time() + 5 * mimosa::minute))
      cleanup();
  }
}
