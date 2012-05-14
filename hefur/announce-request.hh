#ifndef HEFUR_ANNOUNCE_REQUEST_HH
# define HEFUR_ANNOUNCE_REQUEST_HH

# include <cstdint>

# include "address.hh"
# include "info-sha1.hh"

namespace hefur
{
  /**
   * This class represents a generic announce request.
   * It is used by both http(s) server and upd server.
   */
  struct AnnounceRequest
  {
    enum Event
    {
      kNone,
      kStarted,
      kCompleted,
      kStopped,
    };

    char     peerid_[20];
    InfoSha1 info_sha1_;
    Event    event_;
    uint64_t downloaded_;
    uint64_t uploaded_;
    Address  addr_;
  };
}

#endif /* !HEFUR_ANNOUNCE_REQUEST_HH */
