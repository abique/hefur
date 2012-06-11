namespace hefur
{
  inline
  InfoSha1::InfoSha1()
  {
  }

  inline
  InfoSha1::InfoSha1(const char *data)
  {
    ::memcpy(bytes_, data, 20);
  }

  inline m::StringRef
  InfoSha1::bytes() const
  {
    return m::StringRef((const char*)bytes_, 20);
  }
}
