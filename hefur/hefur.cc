#include <cstring>
#include <cerrno>

#include "options.hh"
#include "log.hh"
#include "hefur.hh"
#include "fs-tree-white-list.hh"

namespace hefur
{
  Hefur::Hefur()
    : tdb_(new TorrentDb)
  {
    instance_ = this;

    if (!TORRENT_DIR.empty())
      wl_.reset(new FsTreeWhiteList(TORRENT_DIR, SCAN_INTERVAL * m::second));

    if (UDP_PORT != 0)
    {
      udp_server_.reset(new UdpServer);
      udp_server_->start(UDP_PORT, IPV6);
    }

    if (HTTP_PORT != 0)
    {
      http_server_.reset(new HttpServer);
      http_server_->start(HTTP_PORT, IPV6, "", "");
    }

    if (HTTPS_PORT != 0 && !CERT.empty() && !KEY.empty())
    {
      https_server_.reset(new HttpServer);
      https_server_->start(HTTPS_PORT, IPV6, CERT, KEY);
    }

#ifdef HEFUR_CONTROL_INTERFACE
    if (!CONTROL_SOCKET.empty())
    {
      control_server_ = new ControlServer;
      control_server_->start(CONTROL_SOCKET);
    }
#endif
  }

  Hefur::~Hefur()
  {
    stop_.get();
  }

  void
  Hefur::run()
  {
    stop_.get();
  }

  void
  Hefur::stop()
  {
    stop_.set(true);
  }
}
