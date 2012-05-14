#ifndef HEFUR_SCRAPE_RESPONSE_HH
# define HEFUR_SCRAPE_RESPONSE_HH

namespace hefur
{
  /**
   * This class represents a scrape response.
   * It is used by both http(s) server and upd server.
   */
  struct ScrapeResponse : public mimosa::RefCountable<ScrapeRequest>
  {
    bool        error_;
    std::string error_msg_;

    struct Item
    {
      uint32_t nleechers_;
      uint32_t nseeders_;
      uint32_t ncompleted_;
    };

    std::vector<Item> items_;
  };
}

#endif /* !HEFUR_SCRAPE_RESPONSE_HH */
