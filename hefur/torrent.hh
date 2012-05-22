#ifndef HEFUR_TORRENT_HH
# define HEFUR_TORRENT_HH

# include <string>

# include <mimosa/intrusive-dlist.hh>
# include <mimosa/ref-countable.hh>
# include <mimosa/trie.hh>

# include "info-sha1.hh"
# include "peer.hh"
# include "announce-request.hh"
# include "announce-response.hh"

namespace hefur
{
  class Torrent : public mimosa::RefCountable<Torrent>
  {
  public:
    Torrent(const InfoSha1 &    info_sha1,
            const std::string & name = "",
            const std::string & path = "");
    ~Torrent();

    void cleanup();

    AnnounceResponse::Ptr announce(AnnounceRequest::Ptr request);

    typedef mimosa::IntrusiveDList<Peer, Peer*, &Peer::timeout_hook_> timeouts_type;
    typedef mimosa::Trie<Peer *, Peer::key> peers_type;


    InfoSha1      info_sha1_;
    std::string   name_; // optional, used by StatHandler
    std::string   path_; // optional, for later download
    timeouts_type timeouts_;
    peers_type    peers_;
  };
}

#endif /* !HEFUR_TORRENT_HH */
