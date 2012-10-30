#include <mimosa/options/options.hh>

#include "options.hh"

namespace hefur
{
  const uint32_t & MAX_PEERS = *mo::addOption<uint32_t>(
    "", "max-peers", "the maximum number of peers per torrent",
    30000);

  const uint32_t & MAX_TORRENT_SIZE = *mo::addOption<uint32_t>(
    "", "max-torrent-size", "the maximum torrent size, in MiB",
    10);

  const uint32_t & MAX_TORRENT_NAME = *mo::addOption<uint32_t>(
    "", "max-torrent-name", "the maximum torrent name length to truncate to",
    64);

  const uint32_t & MAX_SCAN_DEPTH = *mo::addOption<uint32_t>(
    "", "max-scan-depth", "the maximum depth while scanning torrent-dir",
    64);

  const uint32_t & MAX_SCAN_INODES = *mo::addOption<uint32_t>(
    "", "max-scan-inodes", "the maximum number of inode to scan while scanning torrent-dir",
    128 * 1024);

  const uint32_t & ANNOUNCE_INTERVAL = *mo::addOption<uint32_t>(
    "", "announce-interval", "the duration in minutes between two announces",
    15);

  const uint32_t & SCRAPE_INTERVAL = *mo::addOption<uint32_t>(
    "", "scrape-interval", "the duration in minutes between two scrapes",
    15);

  const uint32_t & HTTP_TIMEOUT = *mo::addOption<uint32_t>(
    "", "http-timeout", "the number of milliseconds to wait until timeout", 2000);

  const uint16_t & HTTP_PORT = *mo::addOption<uint16_t>(
    "", "http-port", "the port to use, 0 to disable", 6969);
  const uint16_t & HTTPS_PORT = *mo::addOption<uint16_t>(
    "", "https-port", "the port to use, 0 to disable", 6970);
  const uint16_t & UDP_PORT = *mo::addOption<uint16_t>(
    "", "udp-port", "the port to use, 0 to disable", 6969);
  const bool & IPV6 = *mo::addSwitch(
    "", "ipv6", "bind on ipv6 instead of ipv4");
  const bool & ALLOW_PROXY = *mo::addSwitch(
    "", "allow-proxy", "allow the peer to specify its address (so usage of proxies)");
  const bool & DISABLE_PEERS_PAGE = *mo::addSwitch(
    "", "disable-peers-page", "disable the HTTP page /peers, which list torrent's peers");
  const bool & DISABLE_STAT_PAGE = *mo::addSwitch(
    "", "disable-stat-page", "disable the HTTP page /stat, which list served torrents");
  const bool & DISABLE_FILE_PAGE = *mo::addSwitch(
    "", "disable-file-page", "disable the HTTP page /file, which serve .torrent files");

  const std::string & CERT = *mo::addOption<std::string>(
    "", "http-cert", "the path to the certificate", "");
  const std::string & KEY = *mo::addOption<std::string>(
    "", "http-key", "the path to the key", "");

  const std::string & TORRENT_DIR = *mo::addOption<std::string>(
    "", "torrent-dir",
    "the directory containing the allowed torrents,"
    " if empty every torrents are allowed", "");

  const uint32_t & SCAN_INTERVAL = *mo::addOption<uint32_t>(
    "", "scan-interval", "the duration in seconds between two torrent-dir scans",
    60);

  const char * VERSION_MSG = mo::addMessage(
    "", "version", "display the software's version",
    "hefurd " HEFUR_VERSION "\n"
    "Copyright (C) 2012 Alexandre Bique\n"
    "License: MIT\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.");

  const std::string & WWW_DIR = *mo::addOption<std::string>(
    "", "www-dir",
    "the directory containing the web data files (html, css, img)",
    "/usr/share/hefur/www");

  const std::string & CONTROL_SOCKET = *mo::addOption<std::string>(
    "", "control-socket",
    "the path to the control socket",
    "/var/run/hefur/control");
}
