#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#if defined(__linux__)
# include <endian.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__)
# include <sys/endian.h>
#elif defined(__OpenBSD__)
# include <sys/types.h>
# define be16toh(x) betoh16(x)
# define be32toh(x) betoh32(x)
# define be64toh(x) betoh64(x)
#elif defined(_AIX)
# define be16toh(x) ntohs(x)
# define be32toh(x) ntohl(x)
# define be64toh(x) ntohll(x)
# define htobe16(x) htons(x)
# define htobe32(x) htonl(x)
# define htobe64(x) htonll(x)
#endif

#include <cstring>
#include <cerrno>

#include <gnutls/gnutls.h>
#include <gnutls/crypto.h>

#include "hefur.hh"
#include "udp-server.hh"
#include "log.hh"
#include "options.hh"
#include "info-hash.hxx"

#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif

namespace hefur
{
  UdpServer::UdpServer()
    : stop_(false),
      thread_(),
      fd_(-1),
      secrets_(),
      sbufs_(),
      next_timeout_()
  {
  }

  UdpServer::~UdpServer()
  {
    stop();
  }

  bool
  UdpServer::start(uint16_t port,
                   bool     ipv6)
  {
    if (fd_ >= 0)
      stop();

    stop_ = false;

    fd_ = ::socket(ipv6 ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0)
    {
      log->error("failed to create udp socket: %s", strerror(errno));
      return false;
    }

    static const int enable = 1;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    // non block
    if (::fcntl(fd_, F_SETFL, O_NONBLOCK) == -1)
      log->error("fcntl(O_NONBLOCK) failed: %s", strerror(errno));

    if (ipv6)
    {
      // accepts ipv4 connections
      static const int disable = 0;
      ::setsockopt(fd_, IPPROTO_IPV6, IPV6_V6ONLY, &disable, sizeof (disable));

      struct sockaddr_in6 addr;
      ::memset(&addr, 0, sizeof (addr));
      addr.sin6_addr   = ::in6addr_any;
      addr.sin6_family = AF_INET6;
      addr.sin6_port   = htobe16(port);
      if (::bind(fd_, (struct ::sockaddr *)&addr, sizeof (addr)))
      {
        log->error("failed to bind udp socket: %s", strerror(errno));
        return false;
      }
    }
    else
    {
      struct sockaddr_in addr;
      ::memset(&addr, 0, sizeof (addr));
      addr.sin_addr.s_addr = INADDR_ANY;
      addr.sin_family      = AF_INET;
      addr.sin_port        = htobe16(port);
      if (::bind(fd_, (struct ::sockaddr *)&addr, sizeof (addr)))
      {
        log->error("failed to bind udp socket: %s", strerror(errno));
        return false;
      }
    }

    // pre-generate 3 secrets
    for (unsigned i = 0; i < sizeof (secrets_) / sizeof (*secrets_); ++i)
      genSecret();

    thread_.start([this] { this->run(); });

    return true;
  }

  void
  UdpServer::genSecret()
  {
    memmove(secrets_ + 1, secrets_, sizeof (secrets_) - sizeof (*secrets_));
    if (::gnutls_rnd(GNUTLS_RND_NONCE, secrets_, sizeof (*secrets_)))
      log->error("gnutls_rnd() failed");
    next_timeout_ = m::monotonicTimeCoarse() + m::second;
  }

  void
  UdpServer::run()
  {
    struct pollfd pfd;
    int           timeout;

    pfd.fd     = fd_;
    pfd.events = POLLIN;

    while (!stop_)
    {
      timeout = (next_timeout_ - m::monotonicTimeCoarse()) / m::millisecond;
      if (timeout <= 0)
      {
        genSecret();
        continue;
      }

      if (sbufs_.empty())
        pfd.events = POLLIN;
      else
        pfd.events = POLLIN | POLLOUT;

      int ret = ::poll(&pfd, 1, timeout);
      if (ret < 0)
        log->error("poll(): %s", ::strerror(errno));
      else if (ret == 0)
        genSecret();
      else if (pfd.revents & POLLOUT)
        send();
      else if (pfd.revents & POLLIN)
        receive();
    }
  }

  void
  UdpServer::send()
  {
    while (!sbufs_.empty())
    {
      SendToBuffer * sbuf = sbufs_.front();
      ssize_t sbytes = ::sendto(fd_, sbuf->data_, sbuf->len_, MSG_NOSIGNAL,
                                &sbuf->addr_, sizeof (sbuf->in6_));
      if (sbytes < 0)
      {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ||
            errno == ENOMEM || errno == ENOBUFS)
          return;
        log->error("sendto: unexpected error: %s => dropping packet", strerror(errno));
      }
      sbufs_.pop();
      ::free(sbuf);
    }
  }

  void
  UdpServer::receive()
  {
    union
    {
      uint8_t         buffer[4096];
      ConnectRequest  conn;
      AnnounceRequest ann;
      ScrapeRequest   scrape;
    };

    union
    {
      struct ::sockaddr     addr;
      struct ::sockaddr_in  in;
      struct ::sockaddr_in6 in6;
    };

    while (sbufs_.size() < 64)
    {
      socklen_t solen  = sizeof (in6);
      ssize_t   rbytes = recvfrom(fd_, buffer, sizeof (buffer), 0, &addr, &solen);

      if (rbytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
          return;
        log->error("recvfrom: unexpected error: %s", strerror(errno));
        return;
      }

      if ((size_t)rbytes < sizeof (conn))
        continue;

      conn.action_ = be32toh(conn.action_);
      switch (conn.action_) {
      case kConnect:
        handleConnect(&conn, rbytes, &addr, solen);
        break;

      case kAnnounce:
        handleAnnounce(&ann, rbytes, &addr, solen);
        break;

      case kScrape:
        handleScrape(&scrape, rbytes, &addr, solen);
        break;

      default:
        break;
      }
    }
  }

  void
  UdpServer::handleConnect(ConnectRequest *    conn,
                           size_t              size,
                           struct ::sockaddr * addr,
                           socklen_t           addr_len)
  {
    if (size < sizeof (*conn))
      return;

    SendToBuffer * sbuf;
    size_t sbuf_len = sizeof (*sbuf) + sizeof (ConnectResponse);
    sbuf = (SendToBuffer *)calloc(sbuf_len, 1);
    if (!sbuf)
      return;

    sbuf->len_ = sbuf_len - sizeof (SendToBuffer);
    ::memcpy(&sbuf->addr_, addr, addr_len);
    ConnectResponse * rp = (ConnectResponse *)sbuf->data_;
    rp->connection_id_   = connectionId(secrets_[0], addr);
    rp->transaction_id_  = conn->transaction_id_;
    rp->action_          = htobe32(kConnect);
    sbufs_.push(sbuf);
  }

  void
  UdpServer::sendFailure(ConnectRequest *    conn,
                         struct ::sockaddr * addr,
                         socklen_t           addr_len,
                         const std::string & msg)
  {
    SendToBuffer * sbuf;
    size_t sbuf_len = sizeof (*sbuf) + sizeof (ErrorResponse) + msg.size();
    sbuf = (SendToBuffer *)calloc(sbuf_len, 1);
    if (!sbuf)
      return;

    sbuf->len_ = sbuf_len - sizeof (SendToBuffer);
    ::memcpy(&sbuf->addr_, addr, addr_len);
    ErrorResponse * rp = (ErrorResponse *)sbuf->data_;
    rp->transaction_id_  = conn->transaction_id_;
    rp->action_          = htobe32(kError);
    ::memcpy(rp->msg_, msg.data(), msg.size());
    sbufs_.push(sbuf);
  }

  AnnounceRequest::Event
  UdpServer::convert(Event event)
  {
    switch (event)
    {
    case kNone:
      return hefur::AnnounceRequest::kNone;

    case kStarted:
      return hefur::AnnounceRequest::kStarted;

    case kCompleted:
      return hefur::AnnounceRequest::kCompleted;

    case kStopped:
      return hefur::AnnounceRequest::kStopped;

    default:
      return hefur::AnnounceRequest::kNone;
    };
  }

  bool
  UdpServer::checkConnectionId(uint64_t            connection_id,
                               struct ::sockaddr * addr)
  {
    return connection_id == connectionId(secrets_[0], addr) ||
      connection_id == connectionId(secrets_[1], addr) ||
      connection_id == connectionId(secrets_[2], addr);
  }

  void
  UdpServer::handleAnnounce(AnnounceRequest *   ann,
                            size_t              size,
                            struct ::sockaddr * addr,
                            socklen_t           addr_len)
  {
    if (size < sizeof (*ann) ||
        !checkConnectionId(ann->connection_id_, addr))
      return;

    hefur::AnnounceRequest::Ptr rq = new hefur::AnnounceRequest;

    memcpy(rq->peerid_, ann->peer_id_, 20);
    memcpy(rq->info_hash_.bytes_, ann->info_hash_, 20);
    rq->downloaded_ = be64toh(ann->downloaded_);
    rq->uploaded_   = be64toh(ann->uploaded_);
    rq->left_       = be64toh(ann->left_);
    rq->event_      = convert((Event)be32toh(ann->event_));
    rq->num_want_   = be32toh(ann->num_want_);
    rq->skip_ipv6_  = true;
    if (ALLOW_PROXY)
    {
      rq->addr_.family_ = AF_INET;
      memcpy(rq->addr_.in_.addr_, &ann->ip_, 4);
    }
    else
      rq->addr_ = addr;
    rq->addr_.setPort(be16toh(ann->port_));

    auto tdb = Hefur::instance().torrentDb();
    if (!tdb)
      return;
    auto rp = tdb->announce(rq);
    if (!rp || rp->error_)
    {
      sendFailure((ConnectRequest *)ann, addr, addr_len,
                  rp ? rp->error_msg_ : "internal error (1)");
      return;
    }

    SendToBuffer * sbuf;
    size_t sbuf_len = sizeof (*sbuf) + sizeof (AnnounceResponse)
      + 6 * rp->addrs_.size();
    sbuf = (SendToBuffer *)calloc(sbuf_len, 1);
    if (!sbuf)
      return;

    sbuf->len_ = sbuf_len - sizeof (SendToBuffer);
    ::memcpy(&sbuf->addr_, addr, addr_len);
    AnnounceResponse * rp2 = (AnnounceResponse *)sbuf->data_;
    rp2->transaction_id_   = ann->transaction_id_;
    rp2->action_           = htobe32(kAnnounce);
    rp2->interval_         = htobe32(rp->interval_);
    rp2->leechers_         = htobe32(rp->nleechers_);
    rp2->seeders_          = htobe32(rp->nseeders_);
    int i = 0;
    for (auto it = rp->addrs_.begin(); it != rp->addrs_.end(); ++it, ++i)
    {
      if (it->family_ == AF_INET) {
        memcpy(&rp2->peers_[i].ip_, (const char*)&it->in_.addr_, 4);
        memcpy(&rp2->peers_[i].port_, (const char*)&it->in_.port_, 2);
      }
    }
    sbufs_.push(sbuf);
  }

  void
  UdpServer::handleScrape(ScrapeRequest *     scrape,
                          size_t              size,
                          struct ::sockaddr * addr,
                          socklen_t           addr_len)
  {
    if (size < sizeof (*scrape) ||
        !checkConnectionId(scrape->connection_id_, addr))
      return;

    hefur::ScrapeRequest::Ptr rq = new hefur::ScrapeRequest;
    for (size_t i = 0; i < (size - sizeof (*scrape)) / 20; ++i)
      rq->info_hashs_.push_back(InfoHash((const char *)scrape->info_hash_ + i * 20));

    auto tdb = Hefur::instance().torrentDb();
    if (!tdb)
      return;

    auto rp = tdb->scrape(rq);
    if (!rp || rp->error_)
    {
      sendFailure((ConnectRequest *)scrape, addr, addr_len,
                  rp ? rp->error_msg_ : "internal error (1)");
      return;
    }

    SendToBuffer * sbuf;
    size_t sbuf_len = sizeof (*sbuf) + sizeof (ScrapeResponse)
      + 12 * rp->items_.size();
    sbuf = (SendToBuffer *)calloc(sbuf_len, 1);
    if (!sbuf)
      return;

    sbuf->len_ = sbuf_len - sizeof (SendToBuffer);
    ::memcpy(&sbuf->addr_, addr, addr_len);
    ScrapeResponse * rp2 = (ScrapeResponse *)sbuf->data_;
    rp2->transaction_id_ = scrape->transaction_id_;
    rp2->action_         = htobe32(kScrape);

    int i = 0;
    for (auto it = rp->items_.begin(); it != rp->items_.end(); ++it, ++i)
    {
      rp2->torrents_[i].seeders_   = htobe32(it->nseeders_);
      rp2->torrents_[i].leechers_  = htobe32(it->nleechers_);
      rp2->torrents_[i].completed_ = htobe32(it->ndownloaded_);
    }
    sbufs_.push(sbuf);
  }

  void
  UdpServer::stop()
  {
    if (fd_ <= 0)
      return;

    stop_ = true;
    thread_.cancel();
    thread_.join();
  }

  uint64_t
  UdpServer::connectionId(const Secret & secret, struct ::sockaddr * addr)
  {
    if (addr->sa_family == AF_INET)
      return connectionId(secret, (struct ::sockaddr_in *)addr);
    else if (addr->sa_family == AF_INET6)
      return connectionId(secret, (struct ::sockaddr_in6 *)addr);
    // WTF??
    return 42;
  }

  uint64_t
  UdpServer::connectionId(const Secret & secret, struct ::sockaddr_in * addr)
  {
    uint8_t * ip      = (uint8_t *)&addr->sin_addr;
    uint64_t  conn_id = 0;

    for (int j = 0; j < 4; ++j)
      conn_id += (ip[j] * secret.s_[j]) ^ secret.s_[j + 1];
    return conn_id;
  }

  uint64_t
  UdpServer::connectionId(const Secret & secret, struct ::sockaddr_in6 * addr)
  {
    uint8_t * ip      = (uint8_t *)&addr->sin6_addr;
    uint64_t  conn_id = 0;

    for (int j = 0; j < 16; ++j)
      conn_id += (ip[j] * secret.s_[j]) ^ secret.s_[j + 1];
    return conn_id;
  }
}
