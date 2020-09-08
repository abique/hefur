#include <sys/types.h>
#include <signal.h>

#include <mimosa/log/log.hh>

#include "hefur.hh"
#include "control-service.hh"
#include "log.hh"
#include "info-hash.hxx"

namespace hefur
{
  bool
  ControlService::addTorrent(pb::Torrent & request,
                             pb::StatusMsg & response)
  {
    auto torrent = new Torrent(InfoHash(InfoHash::SHA1, request.info_hash().c_str()),
                               request.name(),
                               request.path(),
                               request.length());
    Hefur::instance().torrentDb()->addTorrent(torrent);
    response.set_status(pb::kOk);
    return true;
  }

  bool
  ControlService::removeTorrent(pb::Torrent & request,
                                pb::StatusMsg & response)
  {
    Hefur::instance().torrentDb()->removeTorrent(request.info_hash());
    response.set_status(pb::kOk);
    return true;
  }

  bool
  ControlService::releaseLogs(pb::Void & request,
                              pb::StatusMsg & response)
  {
    ml::release();
    response.set_status(pb::kOk);
    return true;
  }

  bool
  ControlService::quit(pb::Void & request,
                       pb::StatusMsg & response)
  {
    ::kill(::getpid(), SIGQUIT);
    response.set_status(pb::kOk);
    return true;
  }
}
