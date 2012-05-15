#ifndef HEFUR_SCRAPE_HANDLER_HH
# define HEFUR_SCRAPE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  class ScrapeHandler : public mimosa::http::Handler
  {
  public:
    MIMOSA_DEF_PTR(ScrapeHandler);

    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_SCRAPE_HANDLER_HH */
