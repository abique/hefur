#pragma once

#include <mimosa/http/handler.hh>

#include "namespace-helper.hh"

namespace hefur
{
  /**
   * This class handles bittorrent scrape request conforming
   * to the bittorrent specification.
   *
   * This class does convert the http request into the internal
   * representation: ScrapeRequest, and forwards it to TorrentDb.
   */
  class ScrapeHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(ScrapeHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}
