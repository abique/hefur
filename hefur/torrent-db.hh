#ifndef HEFUR_TORRENT_DB_HH
# define HEFUR_TORRENT_DB_HH

# include <mimosa/future.hh>
# include <mimosa/thread.hh>
# include <mimosa/shared-mutex.hh>
# include <mimosa/trie.hh>

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

    void addTorrent(const InfoSha1 & info_sha1,
                    const std::string & name,
                    const std::string & path);

    void addTorrent(const std::string & path);

  private:

    static inline mimosa::StringRef torrentKey(Torrent::Ptr torrent) {
      return mimosa::StringRef(torrent->key());
    }

    typedef mimosa::Trie<Torrent::Ptr, torrentKey> torrents_type;

    void cleanup();
    void cleanupLoop();

    mimosa::Future<bool> cleanup_stop_;
    mimosa::Thread       cleanup_thread_;
    mimosa::SharedMutex  torrents_lock_;
    torrents_type        torrents_;
  };
}

#endif /* !HEFUR_TORRENT_DB_HH */
