#include <mimosa/format/format.hh>
#include <mimosa/format/print.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>

#include "torrent-db.hh"
#include "hefur.hh"
#include "stat-handler.hh"
#include "template-factory.hh"
#include "options.hh"

namespace hefur
{
  bool
  StatHandler::handle(mh::RequestReader &  request,
                      mh::ResponseWriter & response) const
  {
    auto tpl = TemplateFactory::instance().create("page.html");
    if (!tpl)
      return false;

    auto tpl_body = TemplateFactory::instance().create("stat.html");
    if (!tpl_body)
      return false;

    mt::Dict dict;
    HttpServer::commonDict(dict);
    dict.append("body", tpl_body);
    dict.append("title", "Torrents");
    dict.append("tracker_http", mf::str(
                  "http://%v:%v/announce", request.host(), request.port()));
    dict.append("tracker_udp", mf::str(
                  "udp://%v:%v", request.host(), UDP_PORT));

    auto torrents = new mt::List("torrents");
    dict.append(torrents);
    auto tdb = Hefur::instance().torrentDb();
    if (!tdb)
    {
      response.status_ = mh::kStatusServiceUnavailable;
      return true;
    }

    {
      m::SharedMutex::ReadLocker locker(tdb->torrents_lock_);
      uint64_t total_leechers  = 0;
      uint64_t total_seeders   = 0;
      uint64_t total_length    = 0;
      uint64_t total_completed = 0;

      tdb->torrents_.foreach([&] (Torrent *it) {
          auto torrent = new mt::Dict("torrent");
          torrents->append(torrent);
          torrent->append("name", it->name());
          {
            ms::StringStream ss;
            mf::printByteSize(ss, it->length());
            torrent->append("length", ss.str());
          }
          torrent->append("info_sha1", it->key());
          torrent->append("leechers", it->leechers());
          torrent->append("seeders", it->seeders());
          torrent->append("completed", it->completed());
          if (!DISABLE_PEERS_PAGE)
            torrent->append("show_peers", 1);
          if (!DISABLE_FILE_PAGE)
            torrent->append("show_torrent", 1);

          total_leechers  += it->leechers();
          total_seeders   += it->seeders();
          total_length    += it->length();
          total_completed += it->completed();
        });

      dict.append("total_leechers", total_leechers);
      dict.append("total_seeders", total_seeders);
      dict.append("total_completed", total_completed);
      {
        ms::StringStream ss;
        mf::printByteSize(ss, total_length);
        dict.append("total_length", ss.str());
      }
    }

    tpl->execute(&response, dict);
    return true;
  }
}
