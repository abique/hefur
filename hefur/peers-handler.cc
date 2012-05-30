#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/tpl/value.hh>

#include "peers-handler.hh"
#include "torrent-db.hh"
#include "hefur.hh"
#include "template-factory.hh"

namespace hefur
{
  bool
  PeersHandler::handle(mimosa::http::RequestReader &  request,
                       mimosa::http::ResponseWriter & response) const
  {
    auto tpl = TemplateFactory::instance().create("peers.html");
    if (!tpl)
      return false;

    auto & info_hash = request.queryGet("info_hash");
    if (info_hash.size() != 20)
      return false;

    mimosa::tpl::Dict dict;
    auto peers = new mimosa::tpl::List("peers");
    dict.append(peers);
    TorrentDb & tdb = Hefur::instance().torrentDb();

    {
      mimosa::SharedMutex::ReadLocker locker(tdb.torrents_lock_);
      auto torrent = tdb.torrents_.find(info_hash);
      if (!torrent)
      {
        response.status_ = mimosa::http::kStatusNotFound;
        return true;
      }

      for (auto it = torrent->timeouts_.begin(); it != torrent->timeouts_.end(); ++it)
      {
        auto ip = it->addr_.ipStr();
        auto peer = new mimosa::tpl::Dict("peer");
        peers->append(peer);
        peer->append("ip", ip);
        peer->append("port", it->addr_.port());
        peer->append("peerid", mimosa::StringRef((const char *)it->peerid_, 20));
      }

      torrent->peers_.foreach([&peers] (Peer * it) {
          auto ip = it->addr_.ipStr();
          auto peer = new mimosa::tpl::Dict("peer");
          peers->append(peer);
          peer->append("ip", ip);
          peer->append("port", it->addr_.port());
          peer->append("peerid", mimosa::StringRef((const char *)it->peerid_, 20));
        });
    }

    tpl->execute(&response, dict, response.writeTimeout());
    return true;
  }
}
