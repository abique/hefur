#pragma once

#include <memory>

#include <mimosa/condition.hh>
#include <mimosa/mutex.hh>
#include <mimosa/singleton.hh>

#include "fs-tree-white-list.hh"
#include "http-server.hh"
#include "torrent-db.hh"
#include "udp-server.hh"

#ifdef HEFUR_CONTROL_INTERFACE
#include "control-server.hh"
#endif

namespace hefur {
/**
 * This is the main class of Hefur (as you've guessed).
 * It initializes the different components and gracefully
 * shut them down on exit.
 */
class Hefur : public m::Singleton<Hefur>, private m::NonCopyable {
public:
   Hefur();
   ~Hefur();

   void run();
   void stop();

   inline TorrentDb::Ptr torrentDb() const noexcept { return tdb_; }

private:
   m::Future<bool> stop_;
   TorrentDb::Ptr tdb_;
   std::unique_ptr<FsTreeWhiteList> wl_;
   std::unique_ptr<HttpServer> http_server_;
   std::unique_ptr<HttpServer> https_server_;
   std::unique_ptr<UdpServer> udp_server_;

#ifdef HEFUR_CONTROL_INTERFACE
   ControlServer::Ptr control_server_;
#endif
};
} // namespace hefur
