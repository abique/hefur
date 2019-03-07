#pragma once

#include <mimosa/singleton.hh>
#include <mimosa/mutex.hh>
#include <mimosa/condition.hh>

#include "fs-tree-white-list.hh"
#include "torrent-db.hh"
#include "udp-server.hh"
#include "http-server.hh"

#ifdef HEFUR_CONTROL_INTERFACE
# include "control-server.hh"
#endif

namespace hefur
{
  /**
   * This is the main class of Hefur (as you've guessed).
   * It initializes the different components and gracefully
   * shut them down on exit.
   */
  class Hefur : public m::Singleton<Hefur>,
                private m::NonCopyable
  {
  public:
    Hefur();
    ~Hefur();

    void run();
    void stop();

    inline TorrentDb::Ptr torrentDb() { return tdb_; }

  private:
    m::Future<bool>    stop_;
    TorrentDb::Ptr     tdb_;
    FsTreeWhiteList *  wl_;
    HttpServer *       http_server_;
    HttpServer *       https_server_;
    UdpServer *        udp_server_;

#ifdef HEFUR_CONTROL_INTERFACE
    ControlServer::Ptr control_server_;
#endif
  };
}
