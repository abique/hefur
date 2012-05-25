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

  inline mimosa::StringRef
  InfoSha1::bytes() const
  {
    return mimosa::StringRef((const char*)bytes_, 20);
  }
}
