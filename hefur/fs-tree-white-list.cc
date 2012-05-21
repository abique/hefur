#include <fnmatch.h>

#include <mimosa/shared-locker.hh>
#include <mimosa/fs/find.hh>
#include <mimosa/stream/fd-stream.hh>
#include <mimosa/stream/hash.hh>
#include <mimosa/bencode/copy.hh>
#include <mimosa/bencode/encoder.hh>
#include <mimosa/bencode/decoder.hh>

#include "fs-tree-white-list.hh"

namespace hefur
{
  FsTreeWhiteList::FsTreeWhiteList(const std::string & root,
                                   uint32_t            rescan_period)
    : root_(root),
      rescan_period_(rescan_period),
      stop_(),
      sha1s_(),
      sha1s_lock_(),
      scan_thread_([this] { this->loopScan(); })
  {
    scan_thread_.start();
  }

  FsTreeWhiteList::~FsTreeWhiteList()
  {
    stop_.set(true);
    scan_thread_.join();
    sha1s_lock_.lock();
    sha1s_lock_.unlock();
  }

  bool
  FsTreeWhiteList::isAllowed(const InfoSha1 & info_sha1) const
  {
    mimosa::SharedMutex::ReadLocker locker(sha1s_lock_);
    return sha1s_.count(info_sha1) > 0;
  }

  void
  FsTreeWhiteList::scan()
  {
    sha1s_type sha1s;

    mimosa::fs::find(root_, true, [&] (const std::string & path) {
        if (::fnmatch("*.torrent", path.c_str(), FNM_CASEFOLD))
          return;

        loadTorrent(sha1s, path);
      });
  }

  void
  FsTreeWhiteList::loopScan()
  {
    while (!stop_.timedWait(mimosa::time() + mimosa::second * rescan_period_))
      scan();
  }

  void
  FsTreeWhiteList::loadTorrent(sha1s_type & sha1s, const std::string & path)
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
      sha1s.insert(info);
      return;
    }
  }
}
