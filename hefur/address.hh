#ifndef HEFUR_ADDRESS_HH
# define HEFUR_ADDRESS_HH

# include <cstring>
# include <string>

# include <sys/socket.h>
# include <netinet/in.h>

# include <mimosa/string-ref.hh>

namespace hefur
{
  /**
   * Represent an internet address. Both ipv4 and ipv6 are supported.
   */
  struct Address
  {
  public:
    inline Address();
    inline Address(const Address & a);
    inline Address & operator=(const Address & a);

    inline bool operator==(const Address & other) const;

    /**
     * Returns the significant part of the address, to be used as a key.
     * This is helpful for the trie index. For better performances, we
     * choosed to remove family_ from the key, as its length is enough
     * to do the distinction between ipv4 and ipv6, and we moved the port
     * at the end of the string because we can see many peers using the
     * same standard port, but with different addresses.
     */
    inline mimosa::StringRef key() const;

    /**
     * Converts the address to a string representation of the
     * address and the port:
     * - a.b.c.d:port
     * - [ipv6]:port
     */
    std::string str() const;

    /**
     * Converts the address to a string representation of the address.
     */
    std::string ipStr() const;
    uint16_t port() const;

    /**
     * Can be AF_INET or AF_INET6
     */
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
