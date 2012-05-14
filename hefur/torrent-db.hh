#ifndef HEFUR_TORRENT_DB_HH
# define HEFUR_TORRENT_DB_HH

# include "torrent.hh"

namespace hefur
{
  class AnnounceRequest;
  class AnnounceResponse;

  class TorrentDb
  {
  public:
    typedef std::unordered_set<Torrent, InfoSha1::BasicHasher> torrents_type;

    /**
     * Execute the request and store the response in the return value.
     * @warning You are responsible to delete the return value.
     * @return A valid response, but you have to check for null pointer
     * in case of weird internal error.
     */
    AnnounceResponse* annouce(const AnnounceRequest & request);

  private:
    torrents_type torrents_;
  };
}

#endif /* !HEFUR_TORRENT_DB_HH */
