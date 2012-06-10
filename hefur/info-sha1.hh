#ifndef HEFUR_INFO_SHA1_HH
# define HEFUR_INFO_SHA1_HH

# include <cstdint>
# include <cstring>

# include <mimosa/string-ref.hh>

namespace hefur
{
  /**
   * This is just a pure wrapper around 20 bytes (SHA1 hash).
   */
  class InfoSha1
  {
  public:
    inline InfoSha1();
    inline InfoSha1(const char *data);

    inline m::StringRef bytes() const;

    uint8_t bytes_[20];
  };
}

# include "info-sha1.hxx"

#endif /* !HEFUR_INFO_SHA1_HH */
