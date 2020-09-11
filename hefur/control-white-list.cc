#include <mutex>

#include "control-white-list.hh"

namespace hefur {
   void ControlWhiteList::add(const InfoHash &info_hash) {
      std::lock_guard<std::mutex> guard(mutex_);
      info_hashes_.insert(info_hash);
   }

   void ControlWhiteList::remove(const InfoHash &info_hash) {
      std::lock_guard<std::mutex> guard(mutex_);
      info_hashes_.erase(info_hash);
   }

   bool ControlWhiteList::isAllowed(const InfoHash &info_hash) const noexcept
   {
      return info_hashes_.find(info_hash) != info_hashes_.end();
   }
} // namespace hefur