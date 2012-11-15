#include "control-service.hh"

namespace hefur
{
  void
  ControlService::addTorrent(::mimosa::rpc::Call<
                             ::hefur::pb::Torrent,
                             ::hefur::pb::StatusMsg>::Ptr call)
  {
  }

  void
  ControlService::removeTorrent(::mimosa::rpc::Call<
                                ::hefur::pb::Torrent,
                                ::hefur::pb::StatusMsg>::Ptr call)
  {
  }

  void
  ControlService::rotateLogs(::mimosa::rpc::Call<
                             ::hefur::pb::Void,
                             ::hefur::pb::StatusMsg>::Ptr call)
  {
  }

  void
  ControlService::quit(::mimosa::rpc::Call<
                       ::hefur::pb::Void,
                       ::hefur::pb::StatusMsg>::Ptr call)
  {
  }
}
