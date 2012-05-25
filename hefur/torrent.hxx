namespace hefur
{
  inline mimosa::StringRef
  Torrent::key() const
  {
    return info_sha1_.bytes();
  }
}
