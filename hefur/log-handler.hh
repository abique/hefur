#ifndef HEFUR_LOG_HANDLER_HH
# define HEFUR_LOG_HANDLER_HH

#include <mimosa/http/log-handler.hh>
#include <mimosa/http/request-reader.hh>
#include <mimosa/http/request-writer.hh>

namespace hefur {
  class LogHandler : public mh::LogHandler
  {
  public:
    virtual bool handle(mh::RequestReader &request, mh::ResponseWriter &response) const;

    inline void setHandler(Handler::ConstPtr handler) { handler_ = handler; }

  private:
    mh::Handler::ConstPtr handler_;
  };
}
#endif //HEFUR_LOG_HANDLER_HH
