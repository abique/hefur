#pragma once

#include <mutex>
#include <unordered_set>

#include "info-hash.hh"

namespace hefur {

   class ControlWhiteList {
   public:
      void add(const InfoHash &info_hash);
      void remove(const InfoHash &info_hash);

      bool isAllowed(const InfoHash &info_hash) const noexcept;

      void lock() const { mutex_.lock(); }
      void unlock() const { mutex_.unlock(); }

   private:
      mutable std::mutex mutex_;
      std::unordered_set<InfoHash> info_hashes_;
   };

} // namespace hefur