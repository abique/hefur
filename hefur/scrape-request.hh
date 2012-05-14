#ifndef HEFUR_SCRAPE_REQUEST_HH
# define HEFUR_SCRAPE_REQUEST_HH

namespace hefur
{
  /**
   * This class represents a scrape request.
   * It is used by both http(s) server and upd server.
   */
  struct ScrapeRequest : public mimosa::RefCountable<ScrapeRequest>
  {
    std::vector<InfoSha1> info_sha1_;
  };
}

#endif /* !HEFUR_SCRAPE_REQUEST_HH */
