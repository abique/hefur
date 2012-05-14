#ifndef HEFUR_ANNOUNCE_RESPONSE_HH
# define HEFUR_ANNOUNCE_RESPONSE_HH

namespace hefur
{
  /**
   * This class represents a generic announce response.
   * It is used by both http(s) server and upd server.
   */
  struct AnnounceResponse
  {
    bool                 error_;
    std::string          error_msg_;

    uint32_t             nleechers_;
    uint32_t             nseeders_;
    uint32_t             ncompleted_;
    uint32_t             interval_;
    std::vector<Address> addrs_;
  };
}

#endif /* !HEFUR_ANNOUNCE_RESPONSE_HH */
