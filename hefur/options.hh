#ifndef HEFUR_OPTIONS_HH
# define HEFUR_OPTIONS_HH

# include <cstdint>
# include <string>

namespace hefur
{
  extern const uint32_t &    MAX_TORRENT_SIZE;
  extern const uint32_t &    MAX_PEERS;
  extern const uint32_t &    ANNOUNCE_INTERVAL;
  extern const uint32_t &    SCRAPE_INTERVAL;
  extern const uint32_t &    HTTP_TIMEOUT;
  extern const uint16_t &    HTTP_PORT;
  extern const uint16_t &    HTTPS_PORT;
  extern const uint16_t &    UDP_PORT;
  extern const bool &        IPV6;
  extern const std::string & CERT;
  extern const std::string & KEY;
  extern const std::string & TORRENT_DIR;
  extern const uint32_t &    SCAN_INTERVAL;
  extern const std::string & WWW_DIR;
}

#endif /* !HEFUR_OPTIONS_HH */
