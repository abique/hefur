#ifndef HEFUR_TORRENT_DB_HH
# define HEFUR_TORRENT_DB_HH

# include <mimosa/channel.hh>
# include <mimosa/future.hh>
# include <mimosa/thread.hh>

# include "torrent.hh"
# include "announce-request.hh"
# include "announce-response.hh"

namespace hefur
{
  class TorrentDb
  {
  public:
    TorrentDb();
    ~TorrentDb();

    /**
     * Execute the request and store the response in the return value.
     * @return A valid response, but you have to check for null pointer
     * in case of weird internal error.
     */
    AnnounceResponse::Ptr annouce(AnnounceRequest::Ptr request);

  private:
    typedef mimosa::Future<AnnounceResponse::Ptr> future_type;
    typedef std::unordered_set<Torrent, InfoSha1::BasicHasher> torrents_type;

    struct RequestWrapper
    {
      AnnounceRequest::Ptr rq_;
      future_type::Ptr     rp_;
    };

    typedef mimosa::Channel<RequestWrapper> requests_type;

    void run();

    mimosa::Thread thread_;
    requests_type  requests_;
    torrents_type  torrents_;
  };
}

#endif /* !HEFUR_TORRENT_DB_HH */
