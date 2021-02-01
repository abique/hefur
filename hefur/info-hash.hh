#pragma once

#include <cstdint>
#include <string_view>

namespace hefur {
   /** This is just a pure wrapper around N bytes (20 for SHA1 and 32 for SHA2). */
   class InfoHash {
   public:
      enum Type : uint8_t { NONE, SHA1, SHA256 };

      constexpr InfoHash() = default;
      InfoHash(Type type, const char *data);

      friend bool operator==(const InfoHash &a, const InfoHash &b) noexcept {
         if (a.type_ != b.type_)
            return false;
         return a.bytes() == b.bytes();
      }

      inline std::string_view bytes() const noexcept {
         return std::string_view(reinterpret_cast<const char *>(bytes_), hashSize(type_));
      }

      static size_t hashSize(Type type) noexcept;

      static bool isValidHashSize(int size) noexcept;

      uint8_t bytes_[32] = {0};
      Type type_ = NONE;
   };

} // namespace hefur

namespace std {
   template <> struct hash<hefur::InfoHash> {
      std::size_t operator()(hefur::InfoHash const &info_hash) const noexcept {
         return hash<std::string_view>()(info_hash.bytes());
      }
   };
} // namespace std