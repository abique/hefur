#pragma once

#include <mimosa/ref-countable.hh>

#include "info-hash.hh"

namespace hefur {
   /**
    * This class represents a scrape request.
    * It is used by both http(s) server and upd server.
    */
   struct ScrapeRequest : public m::RefCountable<ScrapeRequest> {
      MIMOSA_DEF_PTR(ScrapeRequest);

      std::vector<InfoHash> info_hashs_;
   };
} // namespace hefur
