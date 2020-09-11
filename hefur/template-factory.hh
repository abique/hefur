#pragma once

#include <mimosa/singleton.hh>
#include <mimosa/tpl/cache.hh>

#include "namespace-helper.hh"

namespace hefur {
   /**
    * This is factory which caches parsed html template files.
    */
   class TemplateFactory : public m::Singleton<TemplateFactory> {
   public:
      /**
       * Fetches the template designed by path.
       */
      mt::Template::ConstPtr create(const char *path);

   private:
      mt::Cache cache_;
   };
} // namespace hefur
