#pragma once

#include "torrent.hh"
#include "info-hash.hxx"

namespace hefur
{
  inline m::StringRef
  Torrent::keyV1() const
  {
    return info_hash_v1_.bytes();
  }

  inline m::StringRef
  Torrent::keyV2() const
  {
    return info_hash_v2_.bytes();
  }
}
