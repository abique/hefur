#ifndef HEFUR_TORRENT_DB_HH
# define HEFUR_TORRENT_DB_HH

# include <mimosa/channel.hh>
# include <mimosa/future.hh>
# include <mimosa/thread.hh>

# include "torrent.hh"
# include "announce-request.hh"
# include "announce-response.hh"
# include "scrape-request.hh"
# include "scrape-response.hh"

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
     *
     * @{
     */
    AnnounceResponse::Ptr announce(AnnounceRequest::Ptr request);
    ScrapeResponse::Ptr scrape(ScrapeRequest::Ptr request);
    /** @} */

  private:
    typedef mimosa::Future<AnnounceResponse::Ptr> future_announce_type;
    typedef mimosa::Future<ScrapeResponse::Ptr>   future_scrape_type;

    struct RequestWrapper
    {
      RequestWrapper();
      RequestWrapper(AnnounceRequest::Ptr rq);
      RequestWrapper(ScrapeRequest::Ptr rq);

      AnnounceRequest::Ptr      ann_rq_;
      future_announce_type::Ptr ann_rp_;

      ScrapeRequest::Ptr      scrape_rq_;
      future_scrape_type::Ptr scrape_rp_;
    };

    typedef std::unordered_set<Torrent, InfoSha1::BasicHasher> torrents_type;
    typedef mimosa::Channel<RequestWrapper> requests_type;

    void run();

    mimosa::Thread thread_;
    requests_type  requests_;
    torrents_type  torrents_;
  };
}

#endif /* !HEFUR_TORRENT_DB_HH */
