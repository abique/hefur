#ifndef HEFUR_FILE_HANDLER_HH
# define HEFUR_FILE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace hefur
{
  /**
   * This handler is here to serve local .torrent files.
   * Sample url is: /file/torrent_name.torrent?info_hash=$hex_sha1
   */
  class FileHandler : public mh::Handler
  {
  public:
    MIMOSA_DEF_PTR(FileHandler);

    virtual bool handle(mh::RequestReader & request,
                        mh::ResponseWriter & response) const override;
  };
}

#endif /* !HEFUR_FILE_HANDLER_HH */
