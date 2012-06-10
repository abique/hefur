#include <arpa/inet.h>

#include "address.hh"

namespace hefur
{
  Address::Address()
    : family_(AF_UNSPEC)
  {
  }

  Address::Address(const Address & a)
  {
    *this = a;
  }

  Address &
  Address::operator=(const Address & a)
  {
    ::memcpy(this, &a, sizeof (a));
    return *this;
  }

  bool
  Address::operator==(const Address & other) const
  {
    if (family_ != other.family_)
      return false;

    switch (family_)
    {
    case AF_INET:
      return !::memcmp(&in_, &other.in_, sizeof (in_));

    case AF_INET6:
      return !::memcmp(&in6_, &other.in6_, sizeof (in6_));

    default:
      return false;
    }
  }

  std::string
  Address::str() const
  {
    // ipv4:port or [ipv6]:port
    char buffer[INET6_ADDRSTRLEN + 2 + 1 + 5 + 1] = "(unknown family)";
    char *p = buffer;
    char * const end = buffer + sizeof (buffer);

    switch (family_)
    {
    case AF_INET:
      inet_ntop(AF_INET, in_.addr_, p, end - p);
      p += strlen(p);
      snprintf(p, end - p, ":%d", in_.port_);
      break;

    case AF_INET6:
      *p = '[';
      ++p;
      inet_ntop(AF_INET6, in6_.addr_, p, end - p);
      p += strlen(p);
      snprintf(p, end - p, "]:%d", in6_.port_);
      break;

    default:
      assert(false);
      break;
    }

    return buffer;
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
