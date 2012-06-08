#include <arpa/inet.h>

#include "address.hh"

namespace hefur
{
  std::string
  Address::str() const
  {
    return "XXX";
  }

  std::string
  Address::ipStr() const
  {
    char buffer[INET6_ADDRSTRLEN + 1] = "(unknown family)";

    switch (family_)
    {
    case AF_INET:
      inet_ntop(AF_INET, in_.addr_, buffer, sizeof (buffer));
      break;

    case AF_INET6:
      inet_ntop(AF_INET6, in6_.addr_, buffer, sizeof (buffer));
      break;

    default:
      assert(false);
      break;
    }

    return buffer;
  }

  uint16_t
  Address::port() const
  {
    switch (family_)
    {
    case AF_INET:
      return ntohs(in_.port_);

    case AF_INET6:
      return ntohs(in6_.port_);
    }

    return 0;
  }
}
