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
  PeersHandler::handle(mh::RequestReader &  request,
                       mh::ResponseWriter & response) const
  {
    std::string info_hash = request.queryGet("info_hash");

    ms::StringStream::Ptr ss = new ms::StringStream;
    ms::Base16Decoder::Ptr dec = new ms::Base16Decoder(ss);
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

    mt::Dict dict;
    HttpServer::commonDict(dict);
    dict.append("body", tpl_body);
    dict.append("title", "Torrent peers");

    auto peers = new mt::List("peers");
    dict.append(peers);

    {
      auto tdb = Hefur::instance().torrentDb();
      if (!tdb)
      {
        response.status_ = mh::kStatusServiceUnavailable;
        return true;
      }

      m::SharedMutex::ReadLocker locker(tdb->torrents_lock_);
      auto torrent = tdb->torrents_.find(info_hash);
      if (!torrent)
      {
        response.status_ = mh::kStatusNotFound;
        return true;
      }

      for (auto it = torrent->timeouts_.begin(); it != torrent->timeouts_.end(); ++it)
      {
        auto peer = new mt::Dict("peer");
        peers->append(peer);
        peer->append("address", it->addr_.str());
        peer->append("peerid", m::StringRef((const char *)it->peerid_, 20));

        ms::StringStream ss;
        mf::printByteSize(ss, it->downloaded_);
        peer->append("downloaded", ss.str());

        ss.setStr("");
        mf::printByteSize(ss, it->uploaded_);
        peer->append("uploaded", ss.str());

        ss.setStr("");
        mf::printByteSize(ss, it->left_);
        peer->append("left", ss.str());
      }
    }

    tpl->execute(&response, dict);
    return true;
  }
}
