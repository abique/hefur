#include <mimosa/stream/buffered-stream.hh>
#include <mimosa/stream/net-fd-stream.hh>

#include "control-server.hh"

namespace hefur
{
  ControlServer::ControlServer()
    : mn::Server(),
      stop_(false),
      thread_([this] { this->run(); })
  {
  }

  ControlServer::~ControlServer()
  {
    stop();
  }

  bool
  ControlServer::start(const std::string & socket_path)
  {
    if (!listenUnix(socket_path))
      return false;

    thread_.start();
    return true;
  }

  void
  ControlServer::run()
  {
    while (!stop_)
      serveOne(0, true);
  }

  void
  ControlServer::serve(int                fd,
                       const ::sockaddr * /*address*/,
                       socklen_t          /*address_len*/) const
  {
    ms::NetFdStream::Ptr    net_stream      = new ms::NetFdStream(fd, true);
    ms::BufferedStream::Ptr buffered_stream = new ms::BufferedStream(net_stream);

    while (true) {
      bool            found  = false;
      ms::Buffer::Ptr buffer = buffered_stream->readUntil("\n", 8192, &found);

      if (!buffer || !found)
        return;

      handleCommand(m::StringRef(buffer->data(), buffer->size()));
    }
  }

  void
  ControlServer::stop()
  {
    stop_ = true;
    close();
    thread_.cancel();
    thread_.join();
  }

  void
  ControlServer::handleCommand(m::StringRef cmd) const
  {
    // XXX
  }
}
