#include <mimosa/stream/fd-stream.hh>
#include <mimosa/stream/hash.hh>
#include <mimosa/bencode/copy.hh>
#include <mimosa/bencode/encoder.hh>
#include <mimosa/bencode/decoder.hh>

#include "torrent-db.hh"
#include "log.hh"
#include "options.hh"

namespace hefur
{
  TorrentDb::TorrentDb()
    : cleanup_stop_(),
      cleanup_thread_([this] { this->cleanupLoop(); }),
      torrents_lock_(),
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
    if (!torrent)
    {
      AnnounceResponse::Ptr response = new AnnounceResponse;
      response->error_               = true;
      response->error_msg_           = "torrent not found";
      return response;
    }
    auto response       = torrent->announce(request);
    response->interval_ = ANNOUNCE_INTERVAL * 60;
    return response;
  }

  ScrapeResponse::Ptr
  TorrentDb::scrape(ScrapeRequest::Ptr request)
  {
    ScrapeResponse::Ptr response = new ScrapeResponse;
    response->error_ = false;

    mimosa::SharedMutex::ReadLocker locker(torrents_lock_);
    for (auto it = request->info_sha1s_.begin(); it != request->info_sha1s_.end(); ++it)
    {
      ScrapeResponse::Item item;
      Torrent * torrent = torrents_.find(it->bytes());
      if (!torrent)
        continue;

      item.info_sha1_   = *it;
      item.nleechers_   = torrent->leechers();
      item.nseeders_    = torrent->seeders();
      item.ndownloaded_ = torrent->completed();
      response->items_.push_back(item);
    }
    response->interval_ = SCRAPE_INTERVAL * 60;
    return response;
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

    log->info("creating torrent: %s <- %s", name, path);
    torrent = new Torrent(info_sha1, name, path);
    torrents_.insert(torrent);
  }
}
