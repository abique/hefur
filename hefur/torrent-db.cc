#include "torrent-db.hh"

namespace hefur
{
  TorrentDb::RequestWrapper::RequestWrapper()
  {
  }

  TorrentDb::RequestWrapper::RequestWrapper(AnnounceRequest::Ptr rq)
    : ann_rq_(rq),
      ann_rp_(new future_announce_type)
  {
  }

  TorrentDb::RequestWrapper::RequestWrapper(ScrapeRequest::Ptr rq)
    : scrape_rq_(rq),
      scrape_rp_(new future_scrape_type)
  {
  }

  TorrentDb::TorrentDb()
    : thread_([this] { run(); }),
      requests_(),
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
  TorrentDb::announce(AnnounceRequest::Ptr request)
  {
    RequestWrapper rw(request);
    requests_.push(rw);
    return rw.ann_rp_->get();
  }

  ScrapeResponse::Ptr
  TorrentDb::scrape(ScrapeRequest::Ptr request)
  {
    RequestWrapper rw(request);
    requests_.push(rw);
    return rw.scrape_rp_->get();
  }

  void
  TorrentDb::run()
  {
    RequestWrapper w;

    while (requests_.pop(w))
    {
      // XXX
    }
  }
}
