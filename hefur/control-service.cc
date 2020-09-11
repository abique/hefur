#include <signal.h>
#include <sys/types.h>

#include <mimosa/log/log.hh>

#include "control-service.hh"
#include "hefur.hh"
#include "log.hh"

namespace hefur {
   bool ControlService::releaseLogs(pb::Void &request, pb::StatusMsg &response) {
      ml::release();
      response.set_status(pb::kOk);
      return true;
   }

   bool ControlService::quit(pb::Void &request, pb::StatusMsg &response) {
      ::kill(::getpid(), SIGQUIT);
      response.set_status(pb::kOk);
      return true;
   }
} // namespace hefur
