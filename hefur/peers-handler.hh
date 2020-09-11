#pragma once

#include <mimosa/http/handler.hh>

#include "namespace-helper.hh"

namespace hefur {
   /**
    * This handler generates a web page listing all the torrent's peers.
    */
   class PeersHandler : public mh::Handler {
   public:
      MIMOSA_DEF_PTR(PeersHandler);

      virtual bool handle(mh::RequestReader &request, mh::ResponseWriter &response) const override;
   };
} // namespace hefur
