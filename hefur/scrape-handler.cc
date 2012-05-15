#include <mimosa/bencode/encoder.hh>

#include "hefur.hh"
#include "scrape-handler.hh"

namespace hefur
{
  bool
  ScrapeHandler::handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const
  {
    ScrapeRequest::Ptr rq = new ScrapeRequest;

    auto & query = request.query();

    for (auto it = query.find("info_hash");
         it != query.end() && it->first == "info_hash";
         ++it)
    {
      if (it->second.size() != 20)
        continue;
      rq->info_sha1s_.push_back(InfoSha1(it->second.data()));
    }

    response.content_type_ = "text/plain";
    response.keep_alive_   = false;
    response.sendHeader();
    mimosa::bencode::Encoder enc(&response);

    auto rp = Hefur::instance().torrentDb().scrape(rq);
    if (!rp || rp->error_)
    {
      enc.startDict();
      enc.pushData("failure reason", 14);
      enc.pushData(rp ? rp->error_msg_ : "internal error");
      enc.end();
      return true;
    }

    enc.startDict();
    enc.pushData("files", 5);
    enc.startDict();

    for (auto it = rp->items_.begin(); it != rp->items_.end(); ++it)
    {
      enc.pushData((const char*)it->info_sha1_.bytes_, 20);
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

    return true;
  }
}
