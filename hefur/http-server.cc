#include <cerrno>
#include <cstring>

#include <mimosa/format/print.hh>
#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/uptime.hh>

#include "http-server.hh"
#include "announce-handler.hh"
#include "scrape-handler.hh"
#include "stat-handler.hh"
#include "peers-handler.hh"
#include "log.hh"
#include "options.hh"
#include "file-handler.hh"
#include "log-handler.hh"

namespace hefur
{
  HttpServer::HttpServer()
    : stop_(false),
      thread_([this] { this->run(); }),
      server_(nullptr)
  {
  }

  HttpServer::~HttpServer()
  {
    stop();
  }

  bool
  HttpServer::start(uint16_t            port,
                    bool                ipv6,
                    const std::string & cert,
                    const std::string & key)
  {
    if (server_)
      stop();

    auto dispatch = new mh::DispatchHandler;
    dispatch->registerHandler(
      "/img/*", new mh::FsHandler(
        WWW_DIR + "/img/", 1));
    dispatch->registerHandler(
      "/css/*", new mh::FsHandler(
        WWW_DIR + "/css/", 1));
    dispatch->registerHandler(
      "/js/*", new mh::FsHandler(
        WWW_DIR + "/js/", 1));
    dispatch->registerHandler("/announce", new AnnounceHandler);
    dispatch->registerHandler("/scrape", new ScrapeHandler);
    if (!DISABLE_STAT_PAGE)
      dispatch->registerHandler("/stat", new StatHandler);
    if (!DISABLE_PEERS_PAGE)
      dispatch->registerHandler("/peers", new PeersHandler);
    if (!DISABLE_FILE_PAGE)
      dispatch->registerHandler("/file/*", new FileHandler);

    auto log_handler = new LogHandler;
    log_handler->setHandler(dispatch);

    server_ = new mh::Server;
    server_->setHandler(log_handler);
    server_->setReadTimeout(HTTP_TIMEOUT * m::millisecond);
    server_->setWriteTimeout(HTTP_TIMEOUT * m::millisecond);

    unsigned char buf[sizeof(struct in6_addr)];
    inet_pton(ipv6 ? AF_INET6 : AF_INET, BIND_ADDR.c_str(), buf);
    if ((ipv6 && !server_->listenInet6(port, (in6_addr*)buf)) ||
        (!ipv6 && !server_->listenInet4(port, (in_addr*)buf)))
    {
      log->fatal("failed to listen on the port %d: %s",
                 port, ::strerror(errno));
      return false;
    }

    if (!cert.empty() && !key.empty())
      server_->setSecure(cert, key);

    thread_.start();

    return true;
  }

  void
  HttpServer::run()
  {
    while (!stop_)
      server_->serveOne();
  }

  void
  HttpServer::stop()
  {
    if (!server_)
      return;

    stop_ = true;
    server_->close();
    thread_.cancel();
    thread_.join();
    server_ = nullptr;
  }

  void
  HttpServer::commonDict(mt::Dict & dict)
  {
    {
      ms::StringStream ss;
      mf::printDuration(ss, m::uptime());
      dict.append("hefur_uptime", ss.str());
    }
    dict.append("hefur_version", HEFUR_VERSION);
  }
}
