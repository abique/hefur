#ifndef STAT_HANDLER_HH
# define STAT_HANDLER_HH

# include <mimosa/http/handler.hh>

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

#endif /* !STAT_HANDLER_HH */
