#pragma once

#include "torrent.hh"

namespace hefur
{
  inline m::StringRef
  Torrent::key() const
  {
    return info_hash_.bytes();
  }
}
