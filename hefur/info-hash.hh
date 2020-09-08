#pragma once

#include <cstdint>
#include <cstring>

#include <mimosa/string-ref.hh>

#include "namespace-helper.hh"

namespace hefur
{
  /** This is just a pure wrapper around N bytes (20 for SHA1 and 32 for SHA2). */
  class InfoHash
  {
  public:
    enum Type : uint8_t { NONE, SHA1, SHA256 };

    constexpr InfoHash() = default;
    inline InfoHash(Type type, const char *data);

    inline m::StringRef bytes() const;

    static constexpr size_t hashSize(Type type) noexcept;

    uint8_t bytes_[32] = {0};
    Type    type_ = NONE;
  };
}
