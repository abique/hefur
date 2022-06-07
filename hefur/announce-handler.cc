#include <mimosa/bencode/encoder.hh>
#include <mimosa/http/server-channel.hh>
#include <mimosa/net/print.hh>
#include <mimosa/stream/string-stream.hh>

#include "announce-handler.hh"
#include "hefur.hh"
#include "log.hh"
#include "options.hh"

namespace hefur {
bool AnnounceHandler::handle(mh::RequestReader &request, mh::ResponseWriter &response) const {
   // disable gzip, deflate etc... as some client don't support it
   // even if they claim to
   response.setContentEncoding(mh::kCodingIdentity);

   AnnounceRequest::Ptr rq = new AnnounceRequest;

   auto &peerid = request.queryGet("peer_id");
   auto &info_hash = request.queryGet("info_hash");

   // ensure that peer_id and info_hash has the right size
   if (peerid.size() != 20 || info_hash.size() != 20)
      return false;
   memcpy(rq->peerid_, peerid.data(), 20);
   memcpy(rq->info_hash_.bytes_, info_hash.data(), 20);
   rq->info_hash_.type_ = InfoHash::SHA1;

   // as numwant is optional, we need to check if the key
   // is provided
   auto query_it = request.query().find("numwant");
   if (query_it == request.query().end())
      rq->num_want_ = MIN_NUMWANT;
   else
      rq->num_want_ = std::min<int>(std::atoi(query_it->second.c_str()), MAX_NUMWANT);

   bool compact = request.query().count("compact");
   rq->event_ = AnnounceRequest::parseEvent(request.queryGet("event").data());
   rq->downloaded_ = std::atoll(request.queryGet("downloaded").c_str());
   rq->uploaded_ = std::atoll(request.queryGet("uploaded").c_str());
   rq->left_ = std::atoll(request.queryGet("left").c_str());
   rq->skip_ipv6_ = false;

   // Check for proxy
   auto &ip = request.queryGet("ip");
   if (ALLOW_PROXY && !ip.empty()) {
      struct sockaddr_in in;
      struct sockaddr_in6 in6;

      if (inet_pton(AF_INET, ip.c_str(), &in.sin_addr.s_addr) == 1)
         rq->addr_ = &in;
      else if (inet_pton(AF_INET6, ip.c_str(), &in6.sin6_addr.s6_addr) == 1)
         rq->addr_ = &in6;
   } else
      rq->addr_ = request.channel().remoteAddr();

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
   mb::Encoder enc(buf.get());

   auto tdb = Hefur::instance().torrentDb();
   if (!tdb) {
      response.setStatus(mh::kStatusServiceUnavailable);
      return true;
   }

   // Now that we have converted the HTTP request into an internal
   // representation, let TorrentDb handle the request.
   auto rp = tdb->announce(rq);
   if (!rp || rp->error_) {
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

   if (!compact) {
      enc.pushData("peers", 5);
      enc.startList();
      for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it) {
         enc.startDict();
         enc.pushData("ip", 2);
         enc.pushData(it->ipStr());
         enc.pushData("port", 4);
         enc.pushInt(it->port());
         enc.end();
      }
      enc.end(); // peers
   } else {
      std::string data;
      std::string data6;

      for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it) {
         if (it->family_ == AF_INET) {
            data.append((const char *)&it->in_.addr_, 4);
            data.append((const char *)&it->in_.port_, 2);
         } else if (it->family_ == AF_INET6) {
            data6.append((const char *)&it->in6_.addr_, 16);
            data6.append((const char *)&it->in6_.port_, 2);
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
} // namespace hefur
