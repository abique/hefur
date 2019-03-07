#pragma once

#include "namespace-helper.hh"
#include "address.hh"

namespace hefur
{
  inline m::StringRef
  Address::key() const
  {
    if (family_ == AF_INET)
      return m::StringRef((const char *)&in_, sizeof (in_));
    return m::StringRef((const char *)&in6_, sizeof (in6_));
  }
}
