#ifndef HEFUR_FILE_HANDLER_HH
# define HEFUR_FILE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  class FileHandler : public mimosa::http::Handler
  {
  public:
    MIMOSA_DEF_PTR(FileHandler);

    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_FILE_HANDLER_HH */
