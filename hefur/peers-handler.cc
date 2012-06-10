#include <mimosa/format/print.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/tpl/template.hh>
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
    std::string info_hash = request.queryGet("info_hash");

    mimosa::stream::StringStream::Ptr ss = new mimosa::stream::StringStream;
    mimosa::stream::Base16Decoder::Ptr dec = new mimosa::stream::Base16Decoder(ss);
    dec->write(info_hash.data(), info_hash.size());

    info_hash = ss->str();

    if (info_hash.size() != 20)
      return false;

    auto tpl = TemplateFactory::instance().create("page.html");
    if (!tpl)
      return false;

    auto tpl_body = TemplateFactory::instance().create("peers.html");
    if (!tpl_body)
      return false;

    mimosa::tpl::Dict dict;
    HttpServer::commonDict(dict);
    dict.append("body", tpl_body);
    dict.append("title", "Torrent peers");

    auto peers = new mimosa::tpl::List("peers");
    dict.append(peers);

    {
      TorrentDb & tdb = Hefur::instance().torrentDb();
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

        mimosa::stream::StringStream ss;
        mimosa::format::printByteSize(ss, it->downloaded_);
        dict.append("downloaded", ss.str());

        ss.setStr("");
        mimosa::format::printByteSize(ss, it->uploaded_);
        dict.append("uploaded", ss.str());

        ss.setStr("");
        mimosa::format::printByteSize(ss, it->left_);
        dict.append("left", ss.str());
      }
    }

    tpl->execute(&response, dict);
    return true;
  }
}
