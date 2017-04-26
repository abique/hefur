#include <mimosa/stream/string-stream.hh>
#include <mimosa/bencode/encoder.hh>
#include <mimosa/net/print.hh>
#include <mimosa/http/server-channel.hh>

#include "hefur.hh"
#include "announce-handler.hh"
#include "options.hh"
#include "log.hh"

namespace hefur
{
  bool
  AnnounceHandler::handle(mh::RequestReader & request,
                          mh::ResponseWriter & response) const
  {
    // disable gzip, deflate etc... as some client don't support it
    // even if they claim to
    response.setContentEncoding(mh::kCodingIdentity);

    AnnounceRequest::Ptr rq = new AnnounceRequest;

    auto & peerid = request.queryGet("peer_id");
    auto & info_hash = request.queryGet("info_hash");

    // ensure that peer_id and info_hash has the right size
    if (peerid.size() != 20 || info_hash.size() != 20)
      return false;
    memcpy(rq->peerid_, peerid.data(), 20);
    memcpy(rq->info_hash_.bytes_, info_hash.data(), 20);

    // as numwant is optional, we need to check if the key
    // is provided
    auto query_it = request.query().find("numwant");
    if (query_it == request.query().end())
      rq->num_want_ = 50;
    else
    {
      rq->num_want_ = atoi(query_it->second.c_str());
      // The peer don't need more than 50 peers, so 100
      // is more than enough. It can still try to use PEX
      // to get more peers.
      if (rq->num_want_ > 100)
        rq->num_want_ = 100;
    }

    bool compact = request.query().count("compact");
    rq->event_ = AnnounceRequest::parseEvent(request.queryGet("event").data());
    rq->downloaded_ = atoll(request.queryGet("downloaded").c_str());
    rq->uploaded_ = atoll(request.queryGet("uploaded").c_str());
    rq->left_ = atoll(request.queryGet("left").c_str());
    rq->skip_ipv6_ = false;

    const char *ipaddr_cstr = NULL;
    // Check for reverse proxy
    if (!REVERSE_PROXY_HEADER.empty() && request.unparsedHeaders().find(REVERSE_PROXY_HEADER) != request.unparsedHeaders().end()) {
      ipaddr_cstr = request.unparsedHeaders().find(REVERSE_PROXY_HEADER)->second.c_str();
    }

    // Check for proxy
    auto & ip = request.queryGet("ip");
    if (ALLOW_PROXY && !ip.empty()) {
      ipaddr_cstr = ip.c_str();
    }

    // Set addr_ (from reverse proxy or proxy if existent else the request)
    if (ipaddr_cstr) {
      struct sockaddr_in ip4addr;
      struct sockaddr_in6 ip6addr;

      if (inet_pton(AF_INET, ipaddr_cstr, &(ip4addr.sin_addr)) == 1) {
        ip4addr.sin_family = AF_INET;
        rq->addr_ = (sockaddr*)&ip4addr;
      } else if (inet_pton(AF_INET6, ip.c_str(), &(ip6addr.sin6_addr)) == 1) {
        ip6addr.sin6_family = AF_INET6;
        rq->addr_ = (sockaddr*)&ip6addr;
      }
    } else {
      rq->addr_ = request.channel().remoteAddr();
    }
    rq->addr_.setPort(atoi(request.queryGet("port").c_str()));

    log->info("Got announce from %s", rq->addr_.str());

    // We don't want to keep alive here, because there is no need
    // to let the client queue multiple requests and keep the
    // connection opened.
    response.setKeepAlive(false);
    response.setContentType("text/plain");

    // buffer the response to avoid chunked encoding generating too
    // much noise
    ms::StringStream::Ptr buf = new ms::StringStream;
    mb::Encoder           enc(buf.get());

    auto tdb = Hefur::instance().torrentDb();
    if (!tdb)
    {
      response.setStatus(mh::kStatusServiceUnavailable);
      return true;
    }

    // Now that we have converted the HTTP request into an internal
    // representation, let TorrentDb handle the request.
    auto rp = tdb->announce(rq);
    if (!rp || rp->error_)
    {
      // We failed.
      enc.startDict();
      enc.pushData("failure reason", 14);
      enc.pushData(rp ? rp->error_msg_ : "internal error (1)");
      enc.end();

      // avoid Chunked-Encoding for old client
      response.setContentLength(buf->str().size());
      response.write(buf->str().data(), buf->str().size());
      return true;
    }

    // We succeed to run the announce
    enc.startDict(); // doc
    enc.pushData("interval", 8);
    enc.pushInt(rp->interval_);

    if (!compact)
    {
      enc.pushData("peers", 5);
      enc.startList();
      for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it)
      {
        enc.startDict();
        enc.pushData("ip", 2);
        enc.pushData(it->ipStr());
        enc.pushData("port", 4);
        enc.pushInt(it->port());
        enc.end();
      }
      enc.end(); // peers
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
        } else if (it->family_ == AF_INET6) {
          data6.append((const char*)&it->in6_.addr_, 16);
          data6.append((const char*)&it->in6_.port_, 2);
        }
      }
      enc.pushData("peers", 5);
      enc.pushData(data);
      enc.pushData("peers6", 6);
      enc.pushData(data6);
    }

    enc.end(); // doc

    // avoid Chunked-Encoding for old client which only supports HTTP/1.0
    response.setContentLength(buf->str().size());
    response.write(buf->str().data(), buf->str().size());
    return true;
  }
}
