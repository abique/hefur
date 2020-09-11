#pragma once

#include <memory>

#include "control-white-list.hh"
#include "fs-tree-white-list.hh"
#include "info-hash.hh"

namespace hefur {
   class TorrentDb;

   class MasterWhiteList {
   public:
      void lock();
      void unlock();

      bool isAllowed(const InfoHash &hash) const noexcept;

   private:
      const std::unique_ptr<FsTreeWhiteList> fs_tree_white_list_;
      const std::unique_ptr<ControlWhiteList> control_white_list_;

#ifndef NDEBUG
      bool is_locked_ = false;
#endif
   };
} // namespace hefur