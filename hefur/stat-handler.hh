#pragma once

#include <mimosa/http/handler.hh>

#include "namespace-helper.hh"

namespace hefur
{
  /**
   * This handler generates a web page listing all the torrents.
   */
  class StatHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(StatHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}
