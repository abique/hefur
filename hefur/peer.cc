#include "peer.hh"

namespace hefur
{
  Peer::Peer()
    : addr_(),
      left_(0),
      downloaded_(0),
      uploaded_(0),
      timeout_ts_(0),
      peerid_()
  {
  }
}
