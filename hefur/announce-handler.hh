#pragma once

#include <mimosa/http/handler.hh>

#include "namespace-helper.hh"

namespace hefur
{
  /**
   * This class handles bittorrent announce request conforming
   * to the bittorrent specification.
   *
   * This class does convert the http request into the internal
   * representation: AnnounceRequest, and forwards it to TorrentDb.
   */
  class AnnounceHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(AnnounceHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}
