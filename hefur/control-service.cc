#include <sys/types.h>
#include <signal.h>

#include <mimosa/log/log.hh>

#include "hefur.hh"
#include "control-service.hh"

namespace hefur
{
  void
  ControlService::addTorrent(::mimosa::rpc::Call<
                             ::hefur::pb::Torrent,
                             ::hefur::pb::StatusMsg>::Ptr call)
  {
    auto torrent = new Torrent(call->request().info_hash().c_str(),
                               call->request().name(),
                               call->request().path(),
                               call->request().length());
    Hefur::instance().torrentDb()->addTorrent(torrent);
    call->response().set_status(pb::kOk);
  }

  void
  ControlService::removeTorrent(::mimosa::rpc::Call<
                                ::hefur::pb::Torrent,
                                ::hefur::pb::StatusMsg>::Ptr call)
  {
    Hefur::instance().torrentDb()->removeTorrent(call->request().info_hash());
    call->response().set_status(pb::kOk);
  }

  void
  ControlService::releaseLogs(::mimosa::rpc::Call<
                              ::hefur::pb::Void,
                              ::hefur::pb::StatusMsg>::Ptr call)
  {
    ml::release();
    call->response().set_status(pb::kOk);
  }

  void
  ControlService::quit(::mimosa::rpc::Call<
                       ::hefur::pb::Void,
                       ::hefur::pb::StatusMsg>::Ptr call)
  {
    ::kill(::getpid(), SIGQUIT);
    call->response().set_status(pb::kOk);
  }
}
