#include <cstring>

#include "info-hash.hh"

namespace hefur {
   size_t InfoHash::hashSize(Type type) noexcept {
      switch (type) {
      case SHA1:
         return 20;

      case SHA256:
         return 32;

      default:
         return 0;
      }
   }

   inline InfoHash::InfoHash(Type type, const char *data) : type_(type) {
      std::memcpy(bytes_, data, hashSize(type));
   }
} // namespace hefur
