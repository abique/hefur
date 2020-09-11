#include "master-white-list.hh"
#include "control-white-list.hh"

namespace hefur {

   void MasterWhiteList::lock() {
      assert(!is_locked_);

      if (fs_tree_white_list_)
         fs_tree_white_list_->lock();

      if (control_white_list_)
         control_white_list_->lock();

      is_locked_ = true;
   }

   void MasterWhiteList::unlock() {
      assert(is_locked_);

      if (control_white_list_)
         control_white_list_->unlock();

      if (fs_tree_white_list_)
         fs_tree_white_list_->unlock();

      is_locked_ = false;
   }

   bool MasterWhiteList::isAllowed(const InfoHash &hash) const noexcept {
      assert(is_locked_);

      if (control_white_list_ && control_white_list_->isAllowed(hash))
         return true;

      if (fs_tree_white_list_ && fs_tree_white_list_->isAllowed(hash))
         return true;

      return false;
   }

} // namespace hefur