#ifndef HEFUR_ADDRESS_HH
# define HEFUR_ADDRESS_HH

# include <cstring>

# include <sys/socket.h>
# include <netinet/in.h>

# include <mimosa/string-ref.hh>

namespace hefur
{
  struct Address
  {
  public:
    uint16_t family_;

    union {
      struct {
        uint8_t  addr_[4];
        uint16_t port_;
      } in_;
      struct {
        uint8_t addr_[16];
        uint16_t port_;
      } in6_;
    };

    inline mimosa::StringRef key() const {
      if (family_ == AF_INET)
        return mimosa::StringRef((const char *)&in_, sizeof (in_));
      return mimosa::StringRef((const char *)&in6_, sizeof (in6_));
    }
  } __attribute__((packed));
}

#endif /* !HEFUR_ADDRESS_HH */
