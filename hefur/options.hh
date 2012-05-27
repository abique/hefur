#ifndef HEFUR_OPTIONS_HH
# define HEFUR_OPTIONS_HH

# include <cstdint>
# include <string>

namespace hefur
{
  extern uint32_t &    ANNOUNCE_INTERVAL;
  extern uint32_t &    SCRAPE_INTERVAL;
  extern uint16_t &    HTTP_PORT;
  extern uint16_t &    HTTPS_PORT;
  extern uint16_t &    UDP_PORT;
  extern bool &        IPV6;
  extern std::string & CERT;
  extern std::string & KEY;
  extern std::string & TORRENT_DIR;
  extern uint32_t &    SCAN_INTERVAL;
}

#endif /* !HEFUR_OPTIONS_HH */
