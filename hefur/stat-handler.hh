#ifndef STAT_HANDLER_HH
# define STAT_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  class StatHandler : public mimosa::http::Handler
  {
  public:
    MIMOSA_DEF_PTR(StatHandler);

    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const override;
  };
}

#endif /* !STAT_HANDLER_HH */
