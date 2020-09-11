#include <signal.h>

#include <cerrno>
#include <cstring>

#include <mimosa/init.hh>
#include <mimosa/priviledge-drop.hh>

#include "hefur.hh"
#include "log.hh"
#include "template-factory.hh"

static void quit(int) {
   static bool once = true;

   if (once) {
      hefur::log->info("gracefully quitting");
      hefur::Hefur::instance().stop();
      once = false;
   }
}

int main(int argc, char **argv) {
   mimosa::init(argc, argv);
   mimosa::priviledgeDrop();

   // gracefully quitting
   signal(SIGINT, quit);
   signal(SIGQUIT, quit);

   // ignore SIGPIPE
   signal(SIGPIPE, SIG_IGN);

   // start hefur
   hefur::TemplateFactory::instance();
   hefur::Hefur::instance().run();

   // release everything
   hefur::Hefur::release();
   hefur::TemplateFactory::release();
   mimosa::deinit();

   return 0;
}
