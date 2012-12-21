namespace hefur
{
  inline
  InfoHash::InfoHash()
  {
  }

  inline
  InfoHash::InfoHash(const char *data)
  {
    ::memcpy(bytes_, data, 20);
  }

  inline m::StringRef
  InfoHash::bytes() const
  {
    return m::StringRef((const char*)bytes_, 20);
  }
}
