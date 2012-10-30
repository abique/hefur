#ifndef HEFUR_CONTROL_SERVER_HH
# define HEFUR_CONTROL_SERVER_HH

# include <mimosa/string-ref.hh>
# include <mimosa/thread.hh>
# include <mimosa/net/server.hh>

namespace hefur
{
  /**
   * This is a server which accepts commands from a unix socket,
   * making it possible to control Hefur.
   */
  class ControlServer : public mn::Server
  {
  public:
    MIMOSA_DEF_PTR(ControlServer);

    ControlServer();
    ~ControlServer();

    /**
     * Starts the server. If the server is already started,
     * then it restarts.
     *
     * @return true on success, and false otherwise
     */
    bool start(const std::string & socket_path);

    /**
     * Stops the server, and does nothing if the server is already
     * stopped.
     */
    void stop();

    void handleCommand(m::StringRef cmd) const;

  private:
    void run();

    virtual void serve(int                fd,
                       const ::sockaddr * address,
                       socklen_t          address_len) const override;

    bool            stop_;
    m::Thread       thread_;
  };
}

#endif /* !HEFUR_CONTROL_SERVER_HH */
