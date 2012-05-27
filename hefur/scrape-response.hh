#ifndef HEFUR_SCRAPE_RESPONSE_HH
# define HEFUR_SCRAPE_RESPONSE_HH

namespace hefur
{
  /**
   * This class represents a scrape response.
   * It is used by both http(s) server and upd server.
   */
  struct ScrapeResponse : public mimosa::RefCountable<ScrapeResponse>
  {
    bool        error_;
    std::string error_msg_;

    struct Item
    {
      InfoSha1 info_sha1_;
      uint32_t nleechers_;
      uint32_t nseeders_;
      uint32_t ndownloaded_;
    };

    uint32_t          interval_;
    std::vector<Item> items_;
  };
}

#endif /* !HEFUR_SCRAPE_RESPONSE_HH */
