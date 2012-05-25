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

    /**
     * Handle an announce request and returns an announce response.
     *
     * The return value should never be null.
     */
    AnnounceResponse::Ptr announce(AnnounceRequest::Ptr request);

    /**
     * Removes timed-out peers.
     */
    void cleanup();

    /**
     * Return a key of the info sha1 to use in a Trie.
     */
    inline mimosa::StringRef key() const;

  private:

    /**
     * Create a peer from an announce request and add the peer
     * to peers_ and timeouts_.
     */
    Peer * createPeer(AnnounceRequest::Ptr request);

    /**
     * Remove the peer from peers_ and timeouts_ and update stats.
     */
    void removePeer(Peer * peer);

    typedef mimosa::IntrusiveDList<Peer, Peer*, &Peer::timeout_hook_> timeouts_type;
    typedef mimosa::Trie<Peer *, Peer::key>                           peers_type;


    InfoSha1      info_sha1_;   // this is the torrent key
    std::string   name_;        // optional, used by StatHandler
    std::string   path_;        // optional, for later download
    timeouts_type timeouts_;    // this is a timeout sorted list of peers
    peers_type    peers_;       // this is an index of peers by using their peer id.

    // stats
    uint32_t nleechers_;        // the number of leechers
    uint32_t nseeders_;         // the number of seeders
    uint32_t ncompleted_;       // the number of times a peer completed the download
  };
}

# include "torrent.hxx"

#endif /* !HEFUR_TORRENT_HH */
