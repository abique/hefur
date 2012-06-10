#include <cstring>
#include <cerrno>

#include "options.hh"
#include "log.hh"
#include "hefur.hh"
#include "fs-tree-white-list.hh"

namespace hefur
{
  Hefur::Hefur()
    : mutex_(),
      cond_(),
      stop_(false),
      wl_(nullptr),
      http_server_(nullptr),
      https_server_(nullptr),
      udp_server_(nullptr)
  {
    if (!TORRENT_DIR.empty())
      wl_ = new FsTreeWhiteList(TORRENT_DIR, SCAN_INTERVAL * m::second);

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
