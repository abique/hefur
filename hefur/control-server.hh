#pragma once

#include <mimosa/string-ref.hh>
#include <mimosa/thread.hh>
#include <mimosa/rpc/server.hh>

namespace hefur
{
  /**
   * This is a server which accepts commands from a unix socket,
   * making it possible to control Hefur.
   */
  class ControlServer : public m::RefCountable<ControlServer>
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
    void cmdRemoveTorrent(m::StringRef cmd) const;
    void cmdCleanupTorrents(m::StringRef cmd) const;
    void cmdQuit(m::StringRef cmd) const;

  private:
    void run();

    bool            stop_;
    m::Thread       thread_;
    std::string     socket_path_;
    mr::Server::Ptr server_;
  };
}
