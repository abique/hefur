#ifndef HEFUR_CONTROL_SERVICE_HH
# define HEFUR_CONTROL_SERVICE_HH

# include "hefur.pb.h"

namespace hefur
{
  class ControlService : public pb::Control::Server
  {
  public:
    MIMOSA_DEF_PTR(ControlService);

    virtual bool addTorrent(pb::Torrent & request,
                            pb::StatusMsg & response) override;

    virtual bool removeTorrent(pb::Torrent & request,
                               pb::StatusMsg & response) override;

    virtual bool releaseLogs(pb::Void & request,
                             pb::StatusMsg & response) override;

    virtual bool quit(pb::Void & request,
                      pb::StatusMsg & response) override;
  };
}

#endif /* !HEFUR_CONTROL_SERVICE_HH */
