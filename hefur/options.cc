#include <mimosa/options/options.hh>

#include "options.hh"

namespace hefur
{
  uint32_t & ANNOUNCE_INTERVAL = *mimosa::options::addOption<uint32_t>(
    "", "announce-interval", "the duration in minutes between two announces",
    15);

  uint32_t & SCRAPE_INTERVAL = *mimosa::options::addOption<uint32_t>(
    "", "scrape-interval", "the duration in minutes between two scrapes",
    15);

  uint16_t & HTTP_PORT = *mimosa::options::addOption<uint16_t>(
    "", "http-port", "the port to use, 0 to disable", 6969);
  uint16_t & HTTPS_PORT = *mimosa::options::addOption<uint16_t>(
    "", "https-port", "the port to use, 0 to disable", 6970);
  uint16_t & UDP_PORT = *mimosa::options::addOption<uint16_t>(
    "", "udp-port", "the port to use, 0 to disable", 6969);
  bool & IPV6 = *mimosa::options::addSwitch(
    "", "ipv6", "bind on ipv6 instead of ipv4");

  std::string & CERT = *mimosa::options::addOption<std::string>(
    "", "cert", "the path to the certificate", "");
  std::string & KEY = *mimosa::options::addOption<std::string>(
    "", "key", "the path to the key", "");

  std::string & TORRENT_DIR = *mimosa::options::addOption<std::string>(
    "", "torrent-dir",
    "the directory containing the allowed torrents,"
    " if empty every torrents are allowed", "");

  uint32_t & SCAN_INTERVAL = *mimosa::options::addOption<uint32_t>(
    "", "scan-interval", "the duration in seconds between two torrent-dir scans",
    60);

  const char * VERSION_MSG = mimosa::options::addMessage(
    "", "version", "display the software's version",
    "hefurd " HEFUR_VERSION "\n"
    "Copyright (C) 2012 Alexandre Bique\n"
    "License: MIT\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.");
}
