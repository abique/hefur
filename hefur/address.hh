#ifndef HEFUR_ADDRESS_HH
# define HEFUR_ADDRESS_HH

# include <cstring>

# include <sys/socket.h>
# include <netinet/in.h>

namespace hefur
{
  union Address
  {
  public:

    struct sockaddr_in  in_;
    struct sockaddr_in6 in6_;

    inline bool operator==(const Address & other) const {
      return in_.sin_family == other.in_.sin_family &&
        ((in_.sin_family == AF_INET &&
          !memcmp(&in_, &other.in_, sizeof (in_))) ||
         (in_.sin_family == AF_INET6 &&
          !memcmp(&in6_, &other.in6_, sizeof (in6_))));
    }

    inline bool operator<(const Address & other) const {
      if (in_.sin_family < other.in_.sin_family)
        return true;
      if (in_.sin_family > other.in_.sin_family)
        return false;
      if (in_.sin_family == AF_INET)
        return memcmp(&in_, &other.in_, sizeof (in_)) < 0;
      return memcmp(&in6_, &other.in6_, sizeof (in6_)) < 0;
    }

  };
}

#endif /* !HEFUR_ADDRESS_HH */
