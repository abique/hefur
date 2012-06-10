#ifndef HEFUR_PEERS_HANDLER_HH
# define HEFUR_PEERS_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  class PeersHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(PeersHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_PEERS_HANDLER_HH */
