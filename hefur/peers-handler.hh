#ifndef HEFUR_PEERS_HANDLER_HH
# define HEFUR_PEERS_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  /**
   * This handler generates a web page listing all the torrent's peers.
   */
  class PeersHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(PeersHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_PEERS_HANDLER_HH */
