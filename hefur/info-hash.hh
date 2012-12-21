#ifndef HEFUR_INFO_HASH_HH
# define HEFUR_INFO_HASH_HH

# include <cstdint>
# include <cstring>

# include <mimosa/string-ref.hh>

namespace hefur
{
  /**
   * This is just a pure wrapper around 20 bytes (SHA1 hash).
   */
  class InfoHash
  {
  public:
    inline InfoHash();
    inline InfoHash(const char *data);

    inline m::StringRef bytes() const;

    uint8_t bytes_[20];
  };
}

# include "info-hash.hxx"

#endif /* !HEFUR_INFO_HASH_HH */
