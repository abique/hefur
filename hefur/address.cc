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
      snprintf(p, end - p, ":%d", ntohs(in_.port_));
      break;

    case AF_INET6:
      *p = '[';
      ++p;
      inet_ntop(AF_INET6, in6_.addr_, p, end - p);
      p += strlen(p);
      snprintf(p, end - p, "]:%d", ntohs(in6_.port_));
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

  Address &
  Address::operator=(const struct ::sockaddr * addr)
  {
    if (!addr)
      return *this;

    switch (addr->sa_family)
    {
    case AF_INET:
    {
      const struct ::sockaddr_in * in = (const struct ::sockaddr_in *)addr;
      family_   = AF_INET;
      in_.port_ = in->sin_port;
      memcpy(in_.addr_, &in->sin_addr, 4);
      break;
    }

    case AF_INET6:
    {
      const struct ::sockaddr_in6 * in6 = (const struct ::sockaddr_in6 *)addr;
      // Are we using ipv4 over ipv6
      if (::memcmp("\0\0\0\0\0\0\0\0\0\0\xFF\xFF", &in6->sin6_addr, 12))
      {
        family_    = AF_INET6;
        in6_.port_ = in6->sin6_port;
        memcpy(in6_.addr_, &in6->sin6_addr, 16);
      }
      else
      {
        family_   = AF_INET;
        in_.port_ = in6->sin6_port;
        memcpy(in_.addr_, in6->sin6_addr.s6_addr + 12, 4);
      }
      break;
    }
    }
    return *this;
  }

  void
  Address::setPort(uint16_t port)
  {
    switch (family_)
    {
    case AF_INET:
      in_.port_ = htons(port);
      break;

    case AF_INET6:
      in6_.port_ = htons(port);
      break;
    }
  }
}
