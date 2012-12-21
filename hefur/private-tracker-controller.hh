#ifndef HEFUR_PRIVATE_TRACKER_CONTROLLER_HH
# define HEFUR_PRIVATE_TRACKER_CONTROLLER_HH

# include <string>

namespace hefur
{
  class Address;
  class InfoHash;

  class PrivateTrackerController
  {
  public:
    PrivateTrackerController();
    virtual ~PrivateTrackerController();

    inline const std::string & name() const { return name_; }

    /**
     * Extracts the key from the request.
     * This method is thread safe.
     * @return true on success, false otherwise.
     */
    virtual bool extractKey(mimosa::http::RequestReader & request,
                            std::string *                 key);

    /**
     * Check if the peer identified by key is allowed to access
     * to the Torrent referenced by info_hash.
     * @return true if allowed, false otherwise.
     */
    virtual bool isAllowed(const std::string & key,
                           const InfoSha1 &    info_hash);

    /**
     * Log peer's stats.
     */
    virtual void logPeerStats(const std::string & key,
                              const InfoSha1 &    info_hash,
                              const Address &     address,
                              uint64_t            tx,
                              uint64_t            rx);

  protected:
    std::string name_;
  };
}

#endif /* !HEFUR_PRIVATE_TRACKER_CONTROLLER_HH */
