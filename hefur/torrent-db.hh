#ifndef HEFUR_TORRENT_DB_HH
# define HEFUR_TORRENT_DB_HH

namespace hefur
{
  class TorrentDb
  {
  public:
    /**
     * Execute the request and store the reponse in the return value.
     * @warning You are responsible to delete the return value.
     */
    AnnounceResponse* annouce(const AnnounceRequest & request);
  };
}

#endif /* !HEFUR_TORRENT_DB_HH */
