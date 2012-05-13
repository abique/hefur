#include "udp-server.hh"

namespace hefur
{
  UdpServer::UdpServer()
    : fd_(-1)
  {
  }

  UdpServer::~UdpServer()
  {
    stop();
  }

  bool
  UdpServer::start(uint16_t port,
                   bool     ipv6)
  {
    if (fd_ >= 0)
      stop();

    // XXX

    return true;
  }

  void
  UdpServer::stop()
  {
    if (fd_ <= 0)
      return;
  }
}
