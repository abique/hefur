#include "hefur.hh"
#include "announce-handler.hh"

namespace hefur
{
  bool
  AnnounceHandler::handle(mimosa::http::RequestReader & request,
                          mimosa::http::ResponseWriter & response) const
  {
    AnnounceRequest::Ptr rq = new AnnounceRequest;

    rq->downloaded_ = atoll(request.queryGet("downloaded").c_str());
    rq->uploaded_ = atoll(request.queryGet("uploaded").c_str());
    return true;
  }
}
