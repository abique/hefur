#include "torrent-db.hh"

namespace hefur
{
  TorrentDb::TorrentDb()
    : thread_([this] { run(); }),
      torrents_()
  {
    thread_.start();
  }

  TorrentDb::~TorrentDb()
  {
    requests_.close();
    thread_.join();
  }

  AnnounceResponse::Ptr
  TorrentDb::annouce(AnnounceRequest::Ptr request)
  {
    RequestWrapper rw({request, new future_type});
    requests_.push(rw);
    return rw.rp_->get();
  }

  void
  TorrentDb::run()
  {
    RequestWrapper w;

    while (requests_.pop(w))
    {
      // XXX
      w.rp_->set(nullptr);
    }
  }
}
