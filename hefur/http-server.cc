#include <cerrno>
#include <cstring>

#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>
#include <mimosa/http/log-handler.hh>

#include "http-server.hh"
#include "announce-handler.hh"
#include "scrape-handler.hh"
#include "stat-handler.hh"
#include "peers-handler.hh"
#include "log.hh"
#include "options.hh"
#include "file-handler.hh"

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

    auto dispatch = new mimosa::http::DispatchHandler;
    dispatch->registerHandler(
      "/img/*", new mimosa::http::FsHandler(
        WWW_DIR + "/img/", 1, true));
    dispatch->registerHandler("/announce", new AnnounceHandler);
    dispatch->registerHandler("/scrape", new ScrapeHandler);
    dispatch->registerHandler("/stat", new StatHandler);
    dispatch->registerHandler("/peers", new PeersHandler);
    dispatch->registerHandler("/file/*", new FileHandler);

    auto log_handler = new mimosa::http::LogHandler;
    log_handler->setHandler(dispatch);

    server_ = new mimosa::http::Server;
    server_->setHandler(log_handler);

    if ((ipv6 && !server_->listenInet6(port)) ||
        (!ipv6 && !server_->listenInet4(port)))
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
    thread_.join();
    server_ = nullptr;
  }
}
