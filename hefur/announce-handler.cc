#include <mimosa/stream/string-stream.hh>
#include <mimosa/bencode/encoder.hh>
#include <mimosa/net/print.hh>
#include <mimosa/http/server-channel.hh>

#include "hefur.hh"
#include "announce-handler.hh"

namespace hefur
{
  bool
  AnnounceHandler::handle(mh::RequestReader & request,
                          mh::ResponseWriter & response) const
  {
    AnnounceRequest::Ptr rq = new AnnounceRequest;

    auto & peerid = request.queryGet("peer_id");
    auto & info_hash = request.queryGet("info_hash");

    if (peerid.size() != 20 || info_hash.size() != 20)
      return false;

    auto query_it = request.query().find("numwant");
    if (query_it == request.query().end())
      rq->num_want_ = 50;
    else
      rq->num_want_ = atoi(query_it->second.c_str());

    if (rq->num_want_ == 0)
      rq->num_want_ = 50;
    else if (rq->num_want_ > 100)
      rq->num_want_ = 100;

    uint16_t port = atoi(request.queryGet("port").c_str());
    bool compact = request.query().count("compact");
    memcpy(rq->peerid_, peerid.data(), 20);
    memcpy(rq->info_sha1_.bytes_, info_hash.data(), 20);
    rq->event_ = AnnounceRequest::parseEvent(request.queryGet("event").data());
    rq->downloaded_ = atoll(request.queryGet("downloaded").c_str());
    rq->uploaded_ = atoll(request.queryGet("uploaded").c_str());
    rq->left_ = atoll(request.queryGet("left").c_str());

    const struct ::sockaddr * addr = request.channel().remoteAddr();
    switch (addr->sa_family)
    {
    case AF_INET:
    {
      const struct ::sockaddr_in * in = (const struct ::sockaddr_in *)addr;
      rq->addr_.family_ = AF_INET;
      rq->addr_.in_.port_ = htons(port);
      memcpy(rq->addr_.in_.addr_, &in->sin_addr, 4);
      break;
    }

    case AF_INET6:
    {
      const struct ::sockaddr_in6 * in6 = (const struct ::sockaddr_in6 *)addr;
      rq->addr_.family_ = AF_INET6;
      rq->addr_.in6_.port_ = htons(port);
      memcpy(rq->addr_.in6_.addr_, &in6->sin6_addr, 16);
      break;
    }

    default:
      // XXX set an error message
      return false;
    }

    response.content_type_ = "text/plain";
    response.keep_alive_   = false;

    ms::StringStream::Ptr buf = new ms::StringStream;
    mb::Encoder enc(buf);

    auto rp = Hefur::instance().torrentDb().announce(rq);
    if (!rp || rp->error_)
    {
      enc.startDict();
      enc.pushData("failure reason", 14);
      enc.pushData(rp ? rp->error_msg_ : "internal error (1)");
      enc.end();

      // avoid Chunked-Encoding for old client
      response.content_length_ = buf->str().size();
      response.write(buf->str().data(), buf->str().size());
      return true;
    }

    enc.startDict();
    enc.pushData("interval", 8);
    enc.pushInt(rp->interval_);

    if (!compact)
    {
      enc.pushData("peers", 5);
      for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it)
      {
        enc.pushData("ip", 2);
        enc.pushData("XXX", 3);
        enc.pushData("port", 4);
        enc.pushInt(port);
      }
      enc.end();
    }
    else
    {
      std::string data;
      std::string data6;

      for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it)
      {
        if (it->family_ == AF_INET) {
          data.append((const char*)&it->in_.addr_, 4);
          data.append((const char*)&it->in_.port_, 2);
        } else if (it->family_ != AF_INET6) {
          data.append((const char*)&it->in6_.addr_, 16);
          data.append((const char*)&it->in6_.port_, 2);
        }
      }
      enc.pushData("peers", 5);
      enc.pushData(data);
      enc.pushData("peers6", 6);
      enc.pushData(data6);
    }

    enc.end(); // doc

    // avoid Chunked-Encoding for old client
    response.content_length_ = buf->str().size();
    response.write(buf->str().data(), buf->str().size());
    return true;
  }
}
