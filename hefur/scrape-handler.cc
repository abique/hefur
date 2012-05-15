#include "hefur.hh"
#include "scrape-handler.hh"

namespace hefur
{
  bool
  ScrapeHandler::handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const
  {
    ScrapeRequest::Ptr rq = new ScrapeRequest;

    return true;
  }
}
