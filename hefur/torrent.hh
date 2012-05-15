#ifndef HEFUR_TORRENT_HH
# define HEFUR_TORRENT_HH

# include <unordered_set>

# include "info-sha1.hh"
# include "peer.hh"

namespace hefur
{
  class Torrent
  {
  public:
    typedef mimosa::container::IntrusiveDList<Peer, Peer*, &Peer::timeout_hook_> timeouts_type;
    typedef std::unordered_set<Peer*, Peer::Hash, Peer::Equal> peers_type;

    InfoSha1      info_sha1_;
    std::string   name_; // optional, used by StatHandler
    std::string   path_; // optional, for later download
    timeouts_type timeouts_;
    peers_type    peers_;

    inline operator const InfoSha1 & () const
    {
      return info_sha1_;
    }
  };
}

#endif /* !HEFUR_TORRENT_HH */
