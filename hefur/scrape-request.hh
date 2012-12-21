#ifndef HEFUR_SCRAPE_REQUEST_HH
# define HEFUR_SCRAPE_REQUEST_HH

namespace hefur
{
  /**
   * This class represents a scrape request.
   * It is used by both http(s) server and upd server.
   */
  struct ScrapeRequest : public m::RefCountable<ScrapeRequest>
  {
    std::vector<InfoHash> info_hashs_;
  };
}

#endif /* !HEFUR_SCRAPE_REQUEST_HH */
