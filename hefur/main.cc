#include <cerrno>
#include <cstring>

#include <mimosa/init.hh>

#include "hefur.hh"
#include "template-factory.hh"

int main(int argc, char **argv)
{
  mimosa::init(argc, argv);

  hefur::TemplateFactory::instance();
  hefur::Hefur::instance().run();
  hefur::Hefur::release();
  hefur::TemplateFactory::release();

  mimosa::deinit();
  return 0;
}
