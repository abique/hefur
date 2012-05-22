#ifndef HEFUR_INFO_SHA1_HH
# define HEFUR_INFO_SHA1_HH

# include <cstdint>
# include <cstring>

# include <mimosa/string-ref.hh>

namespace hefur
{
  struct InfoSha1
  {
  public:
    inline InfoSha1()
    {
    }

    inline InfoSha1(const char *data)
    {
      ::memcpy(bytes_, data, 20);
    }

    inline bool operator<(const InfoSha1 & other) const {
      return ::memcmp(bytes_, other.bytes_, 20) < 0;
    }

    inline bool operator==(const InfoSha1 & other) const {
      return !::memcmp(bytes_, other.bytes_, 20);
    }

    inline bool operator>(const InfoSha1 & other) const {
      return ::memcmp(bytes_, other.bytes_, 20) > 0;
    }

    struct Hasher
    {
      inline size_t operator()(const InfoSha1 & sha1) const
      {
        return *(size_t*)sha1.bytes_;
      }
    };

    inline mimosa::StringRef bytes() const {
      return mimosa::StringRef((const char*)bytes_, 20);
    }

    uint8_t bytes_[20];
  };
}

#endif /* !HEFUR_INFO_SHA1_HH */
