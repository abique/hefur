#include <string>

#include <mimosa/init.hh>
#include <mimosa/log/log.hh>
#include <mimosa/net/connect.hh>
#include <mimosa/options/options.hh>
#include <mimosa/rpc/channel.hh>
#include <mimosa/stream/fd-stream.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/bittorrent/torrent-parser.hh>

#include "hefur.pb.h"

namespace hefur
{
  const std::string & PATH = *mo::addOption<std::string>(
    "", "socket", "the path to the control socket", "/var/run/hefur/control");

  const std::vector<std::string> & ADD_TORRENT = *mo::addMultiOption<std::string>(
    "", "add-torrent", "the path to the torrent");

  const std::vector<std::string> & REMOVE_TORRENT = *mo::addMultiOption<std::string>(
    "", "remove-torrent", "the path to a torrent");

  const std::vector<std::string> & REMOVE_TORRENT_HASH = *mo::addMultiOption<std::string>(
    "", "remove-torrent-hash", "the info hash of the torrent in base16");

  const bool & RELEASE_LOGS = *mo::addSwitch("", "release-logs", "release logs");

  const bool & QUIT = *mo::addSwitch("", "quit", "quit hefurd");

  static bool addTorrent(pb::Control::Client & control,
                         const std::string & path)
  {
    m::bittorrent::TorrentParser parser;
    if (!parser.parseFile(path)) {
      ml::error("%s: parse error", path);
      return false;
    }

    auto desc = parser.result();
    auto torrent = new pb::Torrent;

    torrent->set_info_hash(desc->info_hash_.bytes_, 20);
    torrent->set_length(desc->length_);
    torrent->set_name(desc->name_);
    torrent->set_path(path);
    auto call = control.addTorrent(torrent);
    call->wait();
    if (call->isCanceled()) {
      ml::error("%s: add to hefurd canceled", path);
      return false;
    }
    ml::info("%s: add to hefurd succeed", path);
    return true;
  }

  static bool removeTorrent(pb::Control::Client & control,
                            const std::string & path)
  {
    m::bittorrent::TorrentParser parser;
    if (!parser.parseFile(path)) {
      ml::error("%s: parse error", path);
      return false;
    }

    auto desc = parser.result();
    auto torrent = new pb::Torrent;

    torrent->set_info_hash(desc->info_hash_.bytes_, 20);
    auto call = control.removeTorrent(torrent);
    call->wait();
    if (call->isCanceled()) {
      ml::error("%s: add to hefurd canceled", path);
      return false;
    }
    ml::info("%s: add to hefurd succeed", path);
    return true;
  }

  static bool removeTorrentHash(pb::Control::Client & control,
                                const std::string & hash)
  {
    ms::Base16Decoder dec(new ms::StringStream(hash));

    char sha1[20];
    if (dec.read(sha1, 20) != 20) {
      ml::error("%s: failed to decode info hash", hash);
      return false;
    }

    auto torrent = new pb::Torrent;
    torrent->set_info_hash(sha1, 20);
    auto call = control.removeTorrent(torrent);
    call->wait();
    if (call->isCanceled()) {
      ml::error("%s: add to hefurd canceled", hash);
      return false;
    }
    ml::info("%s: add to hefurd succeed", hash);
    return true;
  }

  static bool releaseLogs(pb::Control::Client & control)
  {
    auto call = control.releaseLogs(new pb::Void);
    call->wait();
    if (call->isCanceled()) {
      ml::error("rotate logs canceled");
      return false;
    }
    ml::info("rotate logs succeed");
    return true;
  }

  static bool quit(pb::Control::Client & control)
  {
    auto call = control.quit(new pb::Void);
    call->wait();
    if (call->isCanceled()) {
      ml::error("quit canceled");
      return false;
    }
    ml::info("quit succeed");
    return true;
  }

  static int hefurctl(int argc, char **argv)
  {
    bool succeed = true;

    m::init(argc, argv);

    {
      /* connect to hefur */
      int fd = mn::connectToUnixSocket(PATH, 0);
      if (fd < 0) {
        ml::error("%s: %s", PATH, ::strerror(errno));
        return 1;
      }

      /* establish the rpc channel */
      ms::FdStream::Ptr stream(new ms::FdStream(fd));
      mr::Channel::Ptr channel(new mr::Channel(stream.get()));
      channel->start();

      /* create our service client */
      pb::Control::Client control(channel);

      // add torrents
      for (auto it = ADD_TORRENT.begin(); it != ADD_TORRENT.end(); ++it)
        succeed = addTorrent(control, *it) & succeed;

      // remove torrents
      for (auto it = REMOVE_TORRENT.begin(); it != REMOVE_TORRENT.end(); ++it)
        succeed = removeTorrent(control, *it) & succeed;

      // remove torrents by hash
      for (auto it = REMOVE_TORRENT_HASH.begin(); it != REMOVE_TORRENT_HASH.end(); ++it)
        succeed = removeTorrentHash(control, *it) & succeed;

      // release logs
      if (RELEASE_LOGS)
        succeed = releaseLogs(control) & succeed;

      // quit hefurd
      if (QUIT)
        succeed = quit(control) & succeed;

      channel->close();
    }

    /* deinit */
    m::deinit();
    return succeed != true;
  }
}

int main(int argc, char **argv)
{
  return hefur::hefurctl(argc, argv);
}
