#include "log-handler.hh"
#include "hefur.hh"
#include "options.hh"

#include <mimosa/http/server-channel.hh>
#include <mimosa/http/log.hh>

namespace hefur
{
  bool
  LogHandler::handle(mh::RequestReader & request, mh::ResponseWriter & response) const
  {
    Address original_ip;
    original_ip = request.channel().remoteAddr();
    std::string ip_str;

    struct sockaddr_in in;
    struct sockaddr_in6 in6;

    // Are the reverse proxy settings enabled?
    if (!REVERSE_PROXY_FROM.empty() && !REVERSE_PROXY_HEADER.empty() && REVERSE_PROXY_FROM == original_ip.ipStr()) {
      // Does the original IP match the reverse-proxy-from setting?
      auto headers = request.unparsedHeaders();
      auto header = headers.find(REVERSE_PROXY_HEADER);

      std::string real_ip;
      if (header != headers.end())
        real_ip = header->second;

      if (!real_ip.empty()) {
        Address ip;
        auto port = original_ip.port();
        auto real_ip_cstr = real_ip.c_str();

        if (inet_pton(AF_INET, real_ip_cstr, &(in.sin_addr.s_addr)) == 1) {
          in.sin_family = AF_INET;
          in.sin_port = port;

          request.channel().setRemoteAddr((struct ::sockaddr*)&in, sizeof(in));
          ip = &in;
          ip_str = ip.ipStr();
        }
        else if (inet_pton(AF_INET6, real_ip_cstr, &(in6.sin6_addr.s6_addr)) == 1) {
          // Are we using ipv4 over ipv6?
          if (memcmp("\0\0\0\0\0\0\0\0\0\0\xFF\xFF", &in6.sin6_addr.s6_addr, 12)) {
            in6.sin6_family = AF_INET6;
            in6.sin6_scope_id = 0;
            in6.sin6_port = port;

            request.channel().setRemoteAddr((struct ::sockaddr*)&in6, sizeof(in6));
            ip = &in6;
          }
          else
          {
            in.sin_family = AF_INET;
            in.sin_port = port;

            memcpy(&in.sin_addr.s_addr, in6.sin6_addr.s6_addr + 12, 4);

            request.channel().setRemoteAddr((struct ::sockaddr*)&in, sizeof(in));
            ip = &in;
          }

          ip_str = ip.ipStr();
        }
      }
    }

    auto http_log = mh::http_log;
    http_log->info("%s %s %s:%s",
                   original_ip.ipStr() + (!ip_str.empty() ? " (" + ip_str + ")" : ""),
                   methodString(request.method()), request.host(),
                   request.rawLocation());
    return handler_->handle(request, response);
  }
}
