#pragma once

#include "info-hash.hh"

namespace hefur
{
  constexpr size_t
  InfoHash::hashSize(Type type) noexcept
  {
    switch (type)
    {
    case SHA1:
      return 20;

    case SHA256:
      return 32;

    default:
      return 0;
    }
  }

  inline
  InfoHash::InfoHash(Type type, const char *data)
    : type_(type)
  {
    ::memcpy(bytes_, data, hashSize(type));
  }

  inline m::StringRef
  InfoHash::bytes() const
  {
    return m::StringRef((const char*)bytes_, hashSize(type_));
  }
}
