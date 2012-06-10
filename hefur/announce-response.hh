#ifndef HEFUR_ANNOUNCE_RESPONSE_HH
# define HEFUR_ANNOUNCE_RESPONSE_HH

# include <cstdint>
# include <string>
# include <vector>

# include <mimosa/ref-countable.hh>
# include <mimosa/non-copyable.hh>

# include "address.hh"

namespace hefur
{
  /**
   * This class represents an announce response.
   * It is used by both http(s) server and upd server.
   */
  struct AnnounceResponse : public m::RefCountable<AnnounceResponse>,
                            private m::NonCopyable // should not be copied
  {
    bool                 error_;
    std::string          error_msg_;

    uint32_t             nleechers_;
    uint32_t             nseeders_;
    uint32_t             ncompleted_;
    uint32_t             interval_; // the interval until the next announce in seconds
    std::vector<Address> addrs_;
  };
}

#endif /* !HEFUR_ANNOUNCE_RESPONSE_HH */
