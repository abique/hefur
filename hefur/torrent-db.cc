#include <mimosa/stream/fd-stream.hh>
#include <mimosa/stream/hash.hh>
#include <mimosa/bencode/copy.hh>
#include <mimosa/bencode/encoder.hh>
#include <mimosa/bencode/decoder.hh>

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

  void
  TorrentDb::addTorrent(const std::string & path)
  {
    auto input = mimosa::stream::FdStream::openFile(path.c_str());

    if (!input)
      return;

    mimosa::bencode::Decoder dec(input);
    auto token = dec.pull();
    if (token != mimosa::bencode::kDict)
      return;

    while (true)
    {
      token = dec.pull();
      if (token != mimosa::bencode::kData)
        return;

      if (dec.getData() != "info")
      {
        dec.eatValue();
        continue;
      }

      mimosa::stream::Sha1::Ptr sha1(new mimosa::stream::Sha1);
      mimosa::bencode::Encoder enc(sha1);
      if (!mimosa::bencode::copyValue(dec, enc))
        return;

      InfoSha1 info;
      memcpy(info.bytes_, sha1->digest(), 20);

      // XXX extract the name

      addTorrent(info, "", path);
      return;
    }
  }

  void
  TorrentDb::addTorrent(const InfoSha1 & info_sha1,
                        const std::string & name,
                        const std::string & path)
  {
    mimosa::SharedMutex::Locker locker(torrents_lock_);
    Torrent::Ptr torrent = torrents_.find(info_sha1.bytes());

    if (torrent)
      return;

    torrent = new Torrent(info_sha1, name, path);
    torrents_.insert(torrent);
  }
}
