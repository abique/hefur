#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/tpl/value.hh>

#include "torrent-db.hh"
#include "hefur.hh"
#include "stat-handler.hh"
#include "template-factory.hh"

namespace hefur
{
  bool
  StatHandler::handle(mimosa::http::RequestReader & /*request*/,
                      mimosa::http::ResponseWriter & response) const
  {
    auto tpl = TemplateFactory::instance().create("stat.html");
    if (!tpl)
      return false;

    mimosa::tpl::Dict dict;
    auto torrents = new mimosa::tpl::List("torrents");
    dict.append(torrents);
    TorrentDb & tdb = Hefur::instance().torrentDb();

    {
      mimosa::SharedMutex::ReadLocker locker(tdb.torrents_lock_);
      tdb.torrents_.foreach([&torrents] (Torrent *it) {
          auto torrent = new mimosa::tpl::Dict("torrent");
          torrents->append(torrent);
          torrent->append("name", it->name());
          torrent->append("info_sha1", it->key());
          torrent->append("leechers", it->leechers());
          torrent->append("seeders", it->seeders());
          torrent->append("completed", it->completed());
        });
    }

    tpl->execute(&response, dict, response.writeTimeout());
    return true;
  }
}
