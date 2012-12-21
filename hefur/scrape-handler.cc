#include <mimosa/bencode/encoder.hh>
#include <mimosa/stream/string-stream.hh>

#include "hefur.hh"
#include "scrape-handler.hh"

namespace hefur
{
  bool
  ScrapeHandler::handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const
  {
    // disable gzip, deflate etc... as some client don't support it
    // even if they claim to
    response.content_encoding_ = mh::kCodingIdentity;

    ScrapeRequest::Ptr rq = new ScrapeRequest;

    auto & query = request.query();

    for (auto it = query.find("info_hash");
         it != query.end() && it->first == "info_hash";
         ++it)
    {
      if (it->second.size() != 20)
        continue;
      rq->info_hashs_.push_back(InfoHash(it->second.data()));
    }

    response.content_type_ = "text/plain";
    response.keep_alive_   = false;

    ms::StringStream::Ptr buf = new ms::StringStream;
    mb::Encoder enc(buf);

    auto tdb = Hefur::instance().torrentDb();
    if (!tdb)
    {
      response.status_ = mh::kStatusServiceUnavailable;
      return true;
    }

    auto rp = tdb->scrape(rq);
    if (!rp || rp->error_)
    {
      enc.startDict();
      enc.pushData("failure reason", 14);
      enc.pushData(rp ? rp->error_msg_ : "internal error");
      enc.end();

      // avoid Chunked-Encoding for old client
      response.content_length_ = buf->str().size();
      response.write(buf->str().data(), buf->str().size());
      return true;
    }

    enc.startDict();

    enc.pushData("flags", 5);
    enc.startDict();
    enc.pushData("min_request_interval", 20);
    enc.pushInt(rp->interval_);
    enc.end();

    enc.pushData("files", 5);
    enc.startDict();

    for (auto it = rp->items_.begin(); it != rp->items_.end(); ++it)
    {
      enc.pushData((const char*)it->info_hash_.bytes_, 20);
      enc.startDict();
      enc.pushData("complete", 8);
      enc.pushInt(it->nseeders_);
      enc.pushData("incomplete", 10);
      enc.pushInt(it->nleechers_);
      enc.pushData("downloaded", 10);
      enc.pushInt(it->ndownloaded_);
      enc.end();
    }

    enc.end(); // files
    enc.end(); // doc

    // avoid Chunked-Encoding for old client
    response.content_length_ = buf->str().size();
    response.write(buf->str().data(), buf->str().size());
    return true;
  }
}
