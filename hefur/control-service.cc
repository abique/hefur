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
    auto torrent = new Torrent(InfoHash(InfoHash::SHA1, request.info_hash_v1().c_str()),
                               InfoHash(InfoHash::SHA256, request.info_hash_v2().c_str()),
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
    auto db = Hefur::instance().torrentDb();
    if (request.has_info_hash_v1())
      db->removeTorrent(request.info_hash_v1());
    if (request.has_info_hash_v2())
      db->removeTorrent(request.info_hash_v2());

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
