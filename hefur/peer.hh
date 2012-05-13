#ifndef HEFUR_PEER_HH
# define HEFUR_PEER_HH

# include <cstdint>

namespace hefur
{
  class Peer
  {
  public:

    uint32_t next_;
    uint32_t prev_;
    uint32_t ts_;
    uint32_t to_next_; // timeout

    union {
      struct sockaddr_in  in_;
      struct sockaddr_in6 in6_;
    };
  } __attribute__((packed));
}

#endif /* !HEFUR_PEER_HH */
