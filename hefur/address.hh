#ifndef HEFUR_ADDRESS_HH
# define HEFUR_ADDRESS_HH

# include <cstring>
# include <string>

# include <sys/socket.h>
# include <netinet/in.h>

# include <mimosa/string-ref.hh>

namespace hefur
{
  struct Address
  {
  public:
    inline Address();
    inline Address(const Address & a);
    inline Address & operator=(const Address & a);

    inline bool operator==(const Address & other) const;

    inline mimosa::StringRef key() const;
    std::string str() const;
    std::string ipStr() const;
    uint16_t port() const;

    uint16_t family_;
    union {
      struct {
        uint8_t  addr_[4];
        uint16_t port_;
      } in_ __attribute__((packed));
      struct {
        uint8_t addr_[16];
        uint16_t port_;
      } in6_ __attribute__((packed));
    };

  } __attribute__((packed));
}

# include "address.hxx"

#endif /* !HEFUR_ADDRESS_HH */
