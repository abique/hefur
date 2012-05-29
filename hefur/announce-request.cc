#include <cstring>

#include "announce-request.hh"

namespace hefur
{
  AnnounceRequest::Event
  AnnounceRequest::parseEvent(const char * str)
  {
    if (::strcasecmp(str, "started"))
      return kStarted;
    if (::strcasecmp(str, "completed"))
      return kCompleted;
    if (::strcasecmp(str, "stopped"))
      return kStopped;
    return kNone;
  }
}
