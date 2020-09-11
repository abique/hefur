#pragma once

#include <string>
#include <vector>

#include <mimosa/ref-countable.hh>

#include "info-hash.hh"
#include "namespace-helper.hh"

namespace hefur {
   /**
    * This class represents a scrape response.
    * It is used by both http(s) server and upd server.
    */
   struct ScrapeResponse : public m::RefCountable<ScrapeResponse> {
      MIMOSA_DEF_PTR(ScrapeResponse);

      bool error_;
      std::string error_msg_;

      struct Item {
         InfoHash info_hash_;
         uint32_t nleechers_;
         uint32_t nseeders_;
         uint32_t ndownloaded_;
      };

      uint32_t interval_;
      std::vector<Item> items_;
   };
} // namespace hefur
