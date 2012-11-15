#ifndef HEFUR_CONTROL_SERVICE_HH
# define HEFUR_CONTROL_SERVICE_HH

# include "hefur.pb.h"

namespace hefur
{
  class ControlService : public pb::Control::Server
  {
  public:
    MIMOSA_DEF_PTR(ControlService);

    virtual void addTorrent(::mimosa::rpc::Call<
                            ::hefur::pb::Torrent,
                            ::hefur::pb::StatusMsg>::Ptr call) override;

    virtual void removeTorrent(::mimosa::rpc::Call<
                               ::hefur::pb::Torrent,
                               ::hefur::pb::StatusMsg>::Ptr call) override;

    virtual void rotateLogs(::mimosa::rpc::Call<
                            ::hefur::pb::Void,
                            ::hefur::pb::StatusMsg>::Ptr call) override;

    virtual void quit(::mimosa::rpc::Call<
                      ::hefur::pb::Void,
                      ::hefur::pb::StatusMsg>::Ptr call) override;
  };
}

#endif /* !HEFUR_CONTROL_SERVICE_HH */
