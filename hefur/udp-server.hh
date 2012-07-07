#ifndef HEFUR_UDP_SERVER_HH
# define HEFUR_UDP_SERVER_HH

# include <arpa/inet.h>
# include <sys/socket.h>
# include <endian.h>

# include <cstdint>

# include <mimosa/intrusive-slist.hh>
# include <mimosa/thread.hh>

#include "announce-request.hh"

namespace hefur
{
  /**
   * This is the udp bittorrent tracker.
   *
   * See:
   * - http://www.bittorrent.org/beps/bep_0015.html (protocol)
   *
   * and see @class HttpServer for more documentation.
   */
  class UdpServer
  {
  public:
    UdpServer();
    ~UdpServer();

    /**
     * Starts the server. If the server is already started,
     * then it restarts.
     *
     * @return true on success, and false otherwise
     */
    bool start(uint16_t port,
               bool     ipv6);

    /**
     * Stops the server, and does nothing if the server is already
     * stopped.
     */
    void stop();

  private:

    void genSecret();
    void run();
    void send();
    void receive();

    enum Action
    {
      kConnect  = 0,
      kAnnounce = 1,
      kScrape   = 2,
      kError    = 3,
    };

    enum Event
    {
      kNone      = 0,
      kCompleted = 1,
      kStarted   = 2,
      kStopped   = 3,
    };

    struct ConnectRequest
    {
      uint64_t connection_id_;
      uint32_t action_;
      uint32_t transaction_id_;
    } __attribute__((packed));

    struct ConnectResponse
    {
      uint32_t action_;
      uint32_t transaction_id_;
      uint64_t connection_id_;
    } __attribute__((packed));

    struct AnnounceRequest
    {
      uint64_t connection_id_;
      uint32_t action_;
      uint32_t transaction_id_;
      uint8_t  info_hash_[20];
      uint8_t  peer_id_[20];
      uint64_t downloaded_;
      uint64_t left_;
      uint64_t uploaded_;
      uint32_t event_;
      uint32_t ip_;
      uint32_t key_;
      uint32_t num_want_;
      uint16_t port_;
    } __attribute__((packed));

    struct AnnounceResponse
    {
      uint32_t   action_;
      uint32_t   transaction_id_;
      uint32_t   interval_;
      uint32_t   leechers_;
      uint32_t   seeders_;
      struct {
        uint32_t ip_;
        uint16_t port_;
      } __attribute__((packed)) peers_[0];
    };

    struct ScrapeRequest
    {
      uint64_t connection_id_;
      uint32_t action_;
      uint32_t transaction_id_;
      uint8_t  info_hash_[0];
    } __attribute__((packed));

    struct ScrapeResponse
    {
      uint32_t   action_;
      uint32_t   transaction_id_;
      struct {
        uint32_t seeders_;
        uint32_t completed_;
        uint32_t leechers_;
      } __attribute__((packed)) torrents_[0];
    } __attribute__((packed));

    struct ErrorResponse
    {
      uint32_t action_;
      uint32_t transaction_id_;
      char     msg_[0];
    } __attribute__((packed));

    struct Secret
    {
      uint8_t s_[32];
    };

    struct SendToBuffer
    {
      m::IntrusiveSlistHook<SendToBuffer> lh_; // list hook

      union                     // sendto addr
      {
        struct ::sockaddr     addr_;
        struct ::sockaddr_in  in_;
        struct ::sockaddr_in6 in6_;
      };

      size_t  len_;
      uint8_t data_[0];
    };

    bool checkConnectionId(uint64_t connection_id, struct ::sockaddr * addr);
    static uint64_t connectionId(const Secret & secret, struct ::sockaddr * addr);
    static uint64_t connectionId(const Secret & secret, struct ::sockaddr_in * in);
    static uint64_t connectionId(const Secret & secret, struct ::sockaddr_in6 * in6);

    void handleConnect(ConnectRequest *    conn,
                       size_t              size,
                       struct ::sockaddr * addr,
                       socklen_t           addr_len);

    void handleAnnounce(AnnounceRequest *   ann,
                        size_t              size,
                        struct ::sockaddr * addr,
                        socklen_t           addr_len);

    void handleScrape(ScrapeRequest *     scrape,
                      size_t              size,
                      struct ::sockaddr * addr,
                      socklen_t           addr_len);

    void sendFailure(ConnectRequest *    conn,
                     struct ::sockaddr * addr,
                     socklen_t           addr_len,
                     const std::string & msg);

    static hefur::AnnounceRequest::Event convert(Event event);

    typedef m::IntrusiveSlist<SendToBuffer, &SendToBuffer::lh_> sbufs_type;

    bool       stop_;
    m::Thread  thread_;
    int        fd_;
    Secret     secrets_[3];
    sbufs_type sbufs_;
    m::Time    next_timeout_;
  };
}

#endif /* !HEFUR_UDP_SERVER_HH */
