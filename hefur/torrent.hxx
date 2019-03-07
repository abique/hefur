#pragma once

#include "torrent.hh"
#include "info-hash.hxx"

namespace hefur
{
  inline m::StringRef
  Torrent::key() const
  {
    return info_hash_.bytes();
  }
}
