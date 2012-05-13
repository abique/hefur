#ifndef HEFUR_UDP_SERVER_HH
# define HEFUR_UDP_SERVER_HH

# include <cstdint>

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
    int fd_;
  };
}

#endif /* !HEFUR_UDP_SERVER_HH */
