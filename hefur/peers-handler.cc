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
  StatHandler::handle(mimosa::http::RequestReader & /*request*/,
                      mimosa::http::ResponseWriter & response) const
  {
    auto tpl = TemplateFactory::instance().create("peers.html");
    if (!tpl)
      return false;

    mimosa::tpl::Dict dict;
    auto torrents = new mimosa::tpl::List("peers");
    dict.append(torrents);
    TorrentDb & tdb = Hefur::instance().torrentDb();

    {
      mimosa::SharedMutex::ReadLocker locker(tdb.torrents_lock_);
      // XXX tdb.torrents_.find()
    }

    tpl->execute(&response, dict, response.writeTimeout());
    return true;
  }
}
