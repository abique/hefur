#include <mimosa/http/fs-handler.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>

#include "file-handler.hh"
#include "hefur.hh"
#include "info-hash.hh"
#include "mimosa/http/status.hh"
#include "template-factory.hh"
#include "torrent-db.hh"

namespace hefur {
   bool FileHandler::handle(mh::RequestReader &request, mh::ResponseWriter &response) const {
      // just get and convert the info_hash query parameter
      ms::StringStream::Ptr ss = new ms::StringStream;
      ms::Base16Decoder::Ptr dec = new ms::Base16Decoder(ss.get());
      // XXX probably a gcc bug, change to dec->write(...)
      static_cast<ms::Stream *>(dec.get())->write(request.queryGet("info_hash"));

      const std::string info_hash(ss->str());

      // valid sha1 must be 20 bytes
      if (InfoHash::isValidHashSize(info_hash.size())) {
         response.setStatus(mh::kStatusBadRequest);
         response.setContentType("text");
         return false;
      }

      // fetch the torrent path directly from the torrent db
      std::string path;
      {
         TorrentDb::Ptr tdb = Hefur::instance().torrentDb();
         if (!tdb) {
            response.setStatus(mh::kStatusServiceUnavailable);
            return true;
         }

         m::SharedMutex::ReadLocker locker(tdb->torrents_lock_);
         auto torrent = tdb->torrents_.find(info_hash);
         if (!torrent) {
            response.setStatus(mh::kStatusNotFound);
            return true;
         }

         path = torrent->path();
      }

      // set the content type and reuse streamFile()
      response.setContentType("application/x-bittorrent");
      return mh::FsHandler::streamFile(request, response, path);
   }
} // namespace hefur
