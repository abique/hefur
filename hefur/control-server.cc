#include <mimosa/rpc/service-map.hh>

#include "control-server.hh"
#include "control-service.hh"
#include "log.hh"
#include "hefur.pb.h"

namespace hefur
{
  ControlServer::ControlServer()
    : stop_(false),
      thread_([this] { this->run(); }),
      server_(new mr::Server)
  {
    mr::ServiceMap::Ptr sm = new mr::ServiceMap;
    sm->add(new ControlService);
    server_->setServiceMap(sm.get());
  }

  ControlServer::~ControlServer()
  {
    stop();
  }

  bool
  ControlServer::start(const std::string & socket_path)
  {
    socket_path_ = socket_path;

    if (!server_->listenUnix(socket_path))
      return false;

    stop_ = false;
    thread_.start();
    return true;
  }

  void
  ControlServer::run()
  {
    while (!stop_)
      server_->serveOne(0, false);
  }

  void
  ControlServer::stop()
  {
    stop_ = true;
    server_->close();
    ::unlink(socket_path_.c_str());
    thread_.cancel();
    thread_.join();
  }
}
