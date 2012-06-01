#include <mimosa/stream/string-stream.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>
#include <mimosa/http/fs-handler.hh>

#include "file-handler.hh"
#include "torrent-db.hh"
#include "hefur.hh"
#include "template-factory.hh"

namespace hefur
{
  bool
  FileHandler::handle(mimosa::http::RequestReader &  request,
                      mimosa::http::ResponseWriter & response) const
  {
    std::string info_hash = request.queryGet("info_hash");

    mimosa::stream::StringStream::Ptr ss = new mimosa::stream::StringStream;
    mimosa::stream::Base16Decoder::Ptr dec = new mimosa::stream::Base16Decoder(ss);
    dec->write(info_hash.data(), info_hash.size());

    info_hash = ss->str();

    if (info_hash.size() != 20)
      return false;

    std::string path;
    {
      TorrentDb & tdb = Hefur::instance().torrentDb();
      mimosa::SharedMutex::ReadLocker locker(tdb.torrents_lock_);
      auto torrent = tdb.torrents_.find(info_hash);
      if (!torrent)
      {
        response.status_ = mimosa::http::kStatusNotFound;
        return true;
      }

      path = torrent->path();
    }

    response.content_type_ = "application/x-bittorrent";
    return mimosa::http::FsHandler::streamFile(request, response, path);
  }
}
