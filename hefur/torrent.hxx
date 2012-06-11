namespace hefur
{
  inline m::StringRef
  Torrent::key() const
  {
    return info_sha1_.bytes();
  }
}
