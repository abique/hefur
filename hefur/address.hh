#ifndef HEFUR_ADDRESS_HH
# define HEFUR_ADDRESS_HH

# include <cstring>

# include <sys/socket.h>
# include <netinet/in.h>

# include <mimosa/string-ref.hh>

namespace hefur
{
  union Address
  {
  public:
    struct sockaddr     addr_;
    struct sockaddr_in  in_;
    struct sockaddr_in6 in6_;

    inline mimosa::StringRef key() const {
      if (in_.sin_family == AF_INET)
        return mimosa::StringRef((const char *)&in_.sin_addr, 4);
      return mimosa::StringRef((const char *)&in6_.sin6_addr, 4 * 4);
    }
  };
}

#endif /* !HEFUR_ADDRESS_HH */
