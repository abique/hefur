#include <string>

#include <mimosa/init.hh>
#include <mimosa/log/log.hh>
#include <mimosa/net/connect.hh>
#include <mimosa/options/options.hh>
#include <mimosa/rpc/channel.hh>
#include <mimosa/stream/fd-stream.hh>

#include "hefur.pb.h"

namespace hefur
{
  const std::string & PATH = *mo::addOption<std::string>(
    "", "socket", "the path to the control socket", "/var/run/hefur/control");

  static int hefurctl(int argc, char **argv)
  {
    m::init(argc, argv);

    /* connect to hefur */
    int fd = mn::connectToUnixSocket(PATH, 0);
    if (fd < 0) {
      ml::error("%s: %s", PATH, ::strerror(errno));
      return 1;
    }

    /* establish the rpc channel */
    ms::FdStream::Ptr stream(new ms::FdStream(fd));
    mr::Channel::Ptr channel(new mr::Channel(stream));

    /* create our service client */
    pb::Control::Client control(channel);

    // XXX do the job

    /* deinit */
    m::deinit();
    return 0;
  }
}

int main(int argc, char **argv)
{
  return hefur::hefurctl(argc, argv);
}
