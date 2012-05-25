#ifndef HEFUR_HEFUR_HH
# define HEFUR_HEFUR_HH

# include <mimosa/singleton.hh>
# include <mimosa/mutex.hh>
# include <mimosa/condition.hh>

# include "fs-tree-white-list.hh"
# include "torrent-db.hh"
# include "udp-server.hh"
# include "http-server.hh"

namespace hefur
{
  class Hefur : public mimosa::Singleton<Hefur>
  {
  public:
    Hefur();
    ~Hefur();

    void run();
    void stop();

    inline TorrentDb & torrentDb() { return tdb_; }

  private:
    mimosa::Mutex     mutex_;
    mimosa::Condition cond_;
    bool              stop_;

    TorrentDb         tdb_;
    FsTreeWhiteList * wl_;
    HttpServer *      http_server_;
    HttpServer *      https_server_;
    UdpServer *       udp_server_;
  };
}

#endif /* !HEFUR_HEFUR_HH */
