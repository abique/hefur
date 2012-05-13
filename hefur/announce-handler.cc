#include "announce-handler.hh"

namespace hefur
{
  bool
  AnnounceHandler::handle(mimosa::http::RequestReader & request,
                          mimosa::http::ResponseWriter & response) const
  {
    return false;
  }
}
