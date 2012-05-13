#ifndef HEFUR_PEER_DB_HH
# define HEFUR_PEER_DB_HH

# include <cstdint>

namespace hefur
{
  class PeerDb
  {
  public:

    uint32_t npeers_;
    uint32_t timeout_;
  };
}

#endif /* !HEFUR_PEER_DB_HH */
