#include <string>

#include <mimosa/bittorrent/torrent-parser.hh>
#include <mimosa/init.hh>
#include <mimosa/log/log.hh>
#include <mimosa/net/connect.hh>
#include <mimosa/options/options.hh>
#include <mimosa/rpc/channel.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/stream/fd-stream.hh>
#include <mimosa/stream/string-stream.hh>

#include "hefur.pb.h"
#include "namespace-helper.hh"

namespace hefur {
const std::string &PATH = *mo::addOption<std::string>(
   "", "socket", "the path to the control socket", "/var/run/hefur/control");

const bool &RELEASE_LOGS = *mo::addSwitch("", "release-logs", "release logs");

const bool &QUIT = *mo::addSwitch("", "quit", "quit hefurd");

static bool releaseLogs(pb::Control::Client &control) {
   auto call = control.releaseLogs(new pb::Void);
   call->wait();
   if (call->isCanceled()) {
      ml::error("rotate logs canceled");
      return false;
   }
   ml::info("rotate logs succeed");
   return true;
}

static bool quit(pb::Control::Client &control) {
   auto call = control.quit(new pb::Void);
   call->wait();
   if (call->isCanceled()) {
      ml::error("quit canceled");
      return false;
   }
   ml::info("quit succeed");
   return true;
}

static int hefurctl(int argc, char **argv) {
   bool succeed = true;

   m::init(argc, argv);

   {
      /* connect to hefur */
      int fd = mn::connectToUnixSocket(PATH, 0);
      if (fd < 0) {
         ml::error("%s: %s", PATH, ::strerror(errno));
         return 1;
      }

      /* establish the rpc channel */
      ms::FdStream::Ptr stream(new ms::FdStream(fd));
      mr::Channel::Ptr channel(new mr::Channel(stream.get()));

      /* create our service client */
      pb::Control::Client control(channel);

      // release logs
      if (RELEASE_LOGS)
         succeed = releaseLogs(control) & succeed;

      // quit hefurd
      if (QUIT)
         succeed = quit(control) & succeed;

      channel->close();
   }

   /* deinit */
   m::deinit();
   return succeed != true;
}
} // namespace hefur

int main(int argc, char **argv) { return hefur::hefurctl(argc, argv); }
