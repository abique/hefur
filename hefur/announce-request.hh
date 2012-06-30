#ifndef HEFUR_ANNOUNCE_REQUEST_HH
# define HEFUR_ANNOUNCE_REQUEST_HH

# include <cstdint>

# include <mimosa/ref-countable.hh>
# include <mimosa/non-copyable.hh>

# include "address.hh"
# include "info-sha1.hh"
# include "crypto-support.hh"

namespace hefur
{
  /**
   * This class represents an announce request.
   * It is used by both http(s) server and upd server.
   */
  struct AnnounceRequest : public m::RefCountable<AnnounceRequest>,
                           private m::NonCopyable // should not be copied
  {
    enum Event
    {
      kNone,
      kStarted,
      kCompleted,
      kStopped,
    };

    /**
     * Converts a string to an Event, according to the bittorrent
     * announce event. If we fail to recognize the event type, then
     * we just return kNone.
     */
    static Event parseEvent(const char *str);

    char          peerid_[20];
    InfoSha1      info_sha1_;
    uint32_t      num_want_; // the number of peers wanted
    Event         event_;
    uint64_t      downloaded_;
    uint64_t      uploaded_;
    uint64_t      left_;
    Address       addr_;
    uint16_t      crypto_port_;
    CryptoSupport crypto_support_;
    bool          skip_ipv6_;
  };
}

#endif /* !HEFUR_ANNOUNCE_REQUEST_HH */
