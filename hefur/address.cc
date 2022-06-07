#include <arpa/inet.h>

#include <cassert>

#include "address.hh"

namespace hefur {
Address::Address() : family_(AF_UNSPEC) {}

Address::Address(const Address &a) { *this = a; }

bool Address::operator==(const Address &other) const {
   if (family_ != other.family_)
      return false;

   switch (family_) {
   case AF_INET:
      return !::memcmp(&in_, &other.in_, sizeof(in_));

   case AF_INET6:
      return !::memcmp(&in6_, &other.in6_, sizeof(in6_));

   default:
      return false;
   }
}

std::string Address::str() const {
   // ipv4:port or [ipv6]:port
   char buffer[INET6_ADDRSTRLEN + 2 + 1 + 5 + 1] = "(unknown family)";
   char *p = buffer;
   char *const end = buffer + sizeof(buffer);

   switch (family_) {
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

   case AF_UNSPEC:
      // This can happen
      break;

   default:
      assert(false);
      break;
   }

   return buffer;
}

std::string Address::ipStr() const {
   char buffer[INET6_ADDRSTRLEN + 1] = "(unknown family)";

   switch (family_) {
   case AF_INET:
      inet_ntop(AF_INET, in_.addr_, buffer, sizeof(buffer));
      break;

   case AF_INET6:
      inet_ntop(AF_INET6, in6_.addr_, buffer, sizeof(buffer));
      break;

   case AF_UNSPEC:
      // This can happen
      break;

   default:
      assert(false);
      break;
   }

   return buffer;
}

uint16_t Address::port() const {
   switch (family_) {
   case AF_INET:
      return ntohs(in_.port_);

   case AF_INET6:
      return ntohs(in6_.port_);

   case AF_UNSPEC:
      // This can happen
      return 0;

   default:
      assert(false);
      return 0;
   }
}

Address &Address::operator=(const struct ::sockaddr_in *in) {
   if (!in)
      return *this;

   family_ = AF_INET;
   in_.port_ = in->sin_port;
   memcpy(in_.addr_, &in->sin_addr.s_addr, 4);

   return *this;
}

Address &Address::operator=(const struct ::sockaddr_in6 *in6) {
   if (!in6)
      return *this;

   // Are we using ipv4 over ipv6?
   if (::memcmp("\0\0\0\0\0\0\0\0\0\0\xFF\xFF", &in6->sin6_addr.s6_addr, 12)) {
      family_ = AF_INET6;
      in6_.port_ = in6->sin6_port;
      memcpy(in6_.addr_, &in6->sin6_addr.s6_addr, 16);
   } else {
      family_ = AF_INET;
      in_.port_ = in6->sin6_port;
      memcpy(in_.addr_, in6->sin6_addr.s6_addr + 12, 4);
   }

   return *this;
}

Address &Address::operator=(const struct ::sockaddr *addr) {
   if (!addr)
      return *this;

   switch (addr->sa_family) {
   case AF_INET: {
      *this = reinterpret_cast<const struct ::sockaddr_in *>(addr);
      break;
   }

   case AF_INET6: {
      *this = reinterpret_cast<const struct ::sockaddr_in6 *>(addr);
      break;
   }

   default:
      /* can't work with something else than ipv4 or ipv6 */
      family_ = AF_UNSPEC;
      break;
   }
   return *this;
}

void Address::setPort(uint16_t port) {
   switch (family_) {
   case AF_INET:
      in_.port_ = htons(port);
      break;

   case AF_INET6:
      in6_.port_ = htons(port);
      break;
   }
}
} // namespace hefur
