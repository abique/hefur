#include <signal.h>

#include <cerrno>
#include <cstring>

#include <mimosa/init.hh>

#include "hefur.hh"
#include "log.hh"
#include "template-factory.hh"

static void quit(int)
{
  static bool once = true;

  if (once)
  {
    hefur::log->info("gracefully quitting");
    hefur::Hefur::instance().stop();
    once = false;
  }
}

int main(int argc, char **argv)
{
  m::init(argc, argv);

  // gracefully quitting
  signal(SIGINT, quit);
  signal(SIGQUIT, quit);

  // ignore SIGPIPE
  signal(SIGPIPE, SIG_IGN);

  // start hefur
  hefur::TemplateFactory::instance();
  hefur::Hefur::instance().run();
  hefur::Hefur::release();
  hefur::TemplateFactory::release();

  m::deinit();
  return 0;
}
