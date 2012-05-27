#include <mimosa/stream/string-stream.hh>
#include <mimosa/bencode/encoder.hh>
#include <mimosa/net/print.hh>
#include <mimosa/http/server-channel.hh>

#include "hefur.hh"
#include "announce-handler.hh"

namespace hefur
{
  bool
  AnnounceHandler::handle(mimosa::http::RequestReader & request,
                          mimosa::http::ResponseWriter & response) const
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

    uint16_t port = atoi(request.queryGet("port").c_str());
    bool compact = request.query().count("compact");
    memcpy(rq->peerid_, peerid.data(), 20);
    memcpy(rq->info_sha1_.bytes_, info_hash.data(), 20);
    rq->event_ = AnnounceRequest::parseEvent(request.queryGet("event").data());
    rq->downloaded_ = atoll(request.queryGet("downloaded").c_str());
    rq->uploaded_ = atoll(request.queryGet("uploaded").c_str());
    rq->left_ = atoll(request.queryGet("left").c_str());
    memcpy(&rq->addr_, request.channel().remoteAddr(), request.channel().remoteAddrLen());
    if (rq->addr_.in_.sin_family == AF_INET)
      rq->addr_.in_.sin_port = htons(port);
    else if (rq->addr_.in_.sin_family == AF_INET6)
      rq->addr_.in6_.sin6_port = htons(port);
    else
      return false;

    response.content_type_ = "text/plain";
    response.keep_alive_   = false;

    mimosa::stream::StringStream::Ptr buf = new mimosa::stream::StringStream;
    mimosa::bencode::Encoder enc(buf);

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
        enc.pushData(mimosa::net::print(request.channel().remoteAddr(),
                                        request.channel().remoteAddrLen()));
        enc.pushData("port", 4);
        enc.pushInt(port);
      }
      enc.end();
    }
    else
    {
      std::string data;
      for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it)
      {
        if (it->in_.sin_family != AF_INET)
          continue;

        data.append((const char*)&it->in_.sin_addr, 4);
        data.append((const char*)&it->in_.sin_port, 2);
      }
      enc.pushData("peers", 5);
      enc.pushData(data);

      data.clear();
      for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it)
      {
        if (it->in_.sin_family != AF_INET6)
          continue;

        data.append((const char*)&it->in6_.sin6_addr, 16);
        data.append((const char*)&it->in6_.sin6_port, 2);
      }
      enc.pushData("peers6", 6);
      enc.pushData(data);
    }

    enc.end(); // doc

    // avoid Chunked-Encoding for old client
    response.content_length_ = buf->str().size();
    response.write(buf->str().data(), buf->str().size());
    return true;
  }
}
