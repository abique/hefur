#include <cstring>
#include <cerrno>

#include <mimosa/options/options.hh>

#include "log.hh"
#include "hefur.hh"
#include "fs-tree-white-list.hh"

auto & HTTP_PORT = *mimosa::options::addOption<uint16_t>(
  "", "http-port", "the port to use, 0 to disable", 6969);
auto & HTTPS_PORT = *mimosa::options::addOption<uint16_t>(
  "", "https-port", "the port to use, 0 to disable", 6970);
auto & UDP_PORT = *mimosa::options::addOption<uint16_t>(
  "", "udp-port", "the port to use, 0 to disable", 6969);
auto & IPV6 = *mimosa::options::addSwitch(
  "", "ipv6", "use ipv6 instead of ipv4");

auto & CERT = *mimosa::options::addOption<std::string>(
  "", "cert", "the path to the certificate", "");
auto & KEY = *mimosa::options::addOption<std::string>(
  "", "key", "the path to the key", "");

auto & TORRENT_DIR = *mimosa::options::addOption<std::string>(
  "", "torrent-dir",
  "the directory containing the allowed torrents,"
  " if empty every torrents are allowed", "");
auto & SCAN_PERIOD = *mimosa::options::addOption<uint32_t>(
  "", "scan-period", "the duration in seconds between two torrent-dir scans",
  60);

namespace hefur
{
  Hefur::Hefur()
    : mutex_(),
      cond_(),
      stop_(false),
      pdb_(),
      wl_(nullptr),
      http_server_(nullptr),
      https_server_(nullptr),
      udp_server_(nullptr)
  {
    if (!TORRENT_DIR.empty())
      wl_ = new FsTreeWhiteList(TORRENT_DIR, SCAN_PERIOD);

    if (UDP_PORT != 0)
    {
      udp_server_ = new UdpServer;
      udp_server_->start(UDP_PORT, IPV6);
    }

    if (HTTP_PORT != 0)
    {
      http_server_ = new HttpServer;
      http_server_->start(HTTP_PORT, IPV6, "", "");
    }

    if (HTTPS_PORT != 0 && !CERT.empty() && !KEY.empty())
    {
      https_server_ = new HttpServer;
      https_server_->start(HTTP_PORT, IPV6, CERT, KEY);
    }
  }

  Hefur::~Hefur()
  {
    delete udp_server_;
    delete https_server_;
    delete http_server_;
    delete wl_;
  }

  void
  Hefur::run()
  {
    mimosa::Mutex::Locker locker(mutex_);
    if (stop_)
      return;
    cond_.wait(mutex_);
  }

  void
  Hefur::stop()
  {
    mimosa::Mutex::Locker locker(mutex_);
    cond_.wakeAll();
    stop_ = true;
  }
}
