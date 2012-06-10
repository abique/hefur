#ifndef HEFUR_ANNOUNCE_HANDLER_HH
# define HEFUR_ANNOUNCE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  /**
   * This class handles bittorrent announce request conforming
   * to the bittorrent specification.
   *
   * This class does convert the http request to the internal
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

#endif /* !HEFUR_ANNOUNCE_HANDLER_HH */
