#include <cerrno>
#include <cstring>

#include <mimosa/init.hh>

#include "hefur.hh"

int main(int argc, char **argv)
{
  mimosa::init(argc, argv);

  hefur::Hefur::instance().run();
  hefur::Hefur::release();

  mimosa::deinit();
  return 0;
}
