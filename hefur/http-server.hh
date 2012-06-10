#ifndef HEFUR_HTTP_SERVER_HH
# define HEFUR_HTTP_SERVER_HH

# include <mimosa/thread.hh>
# include <mimosa/http/server.hh>
# include <mimosa/tpl/dict.hh>

namespace hefur
{
  /**
   * This is the Http bittorrent tracker.
   *
   * See:
   * - http://www.bittorrent.org/beps/bep_0003.html (standard interface)
   * - http://www.bittorrent.org/beps/bep_0023.html (compact peers)
   * - http://www.bittorrent.org/beps/bep_0007.html (ipv6)
   * - http://www.bittorrent.org/beps/bep_0022.html (local tracker discovery)
   * - http://www.bittorrent.org/beps/bep_0024.html (external ip)
   * - http://www.bittorrent.org/beps/bep_0027.html (private tracker)
   * - http://www.bittorrent.org/beps/bep_0028.html (tracker exchange)
   * - http://www.bittorrent.org/beps/bep_0031.html (failure retry)
   * - http://www.bittorrent.org/beps/bep_0008.html (tracker peer obfuscation)
   *   yet this one is deferred and not very useful as we have https support.
   *
   * see @class UdpServer for more documentation.
   */
  class HttpServer
  {
  public:
    HttpServer();
    ~HttpServer();

    /**
     * Starts the server. If the server is already started,
     * then it restarts.
     *
     * @return true on success, and false otherwise
     */
    bool start(uint16_t            port,
               bool                ipv6,
               const std::string & cert,
               const std::string & key);

    /**
     * Stops the server, and does nothing if the server is already
     * stopped.
     */
    void stop();

    static void commonDict(mimosa::tpl::Dict & dict);

  private:
    void run();

    bool                      stop_;
    mimosa::Thread            thread_;
    mh::Server::Ptr server_;
  };
}

#endif /* !HEFUR_HTTP_SERVER_HH */
