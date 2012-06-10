#ifndef HEFUR_FILE_HANDLER_HH
# define HEFUR_FILE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  class FileHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(FileHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_FILE_HANDLER_HH */
