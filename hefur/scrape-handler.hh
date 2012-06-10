#ifndef HEFUR_SCRAPE_HANDLER_HH
# define HEFUR_SCRAPE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  class ScrapeHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(ScrapeHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_SCRAPE_HANDLER_HH */
