#ifndef HEFUR_ANNOUNCE_HANDLER_HH
# define HEFUR_ANNOUNCE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  class AnnounceHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(AnnounceHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_ANNOUNCE_HANDLER_HH */
