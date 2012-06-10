namespace hefur
{
  inline mimosa::StringRef
  Address::key() const
  {
    if (family_ == AF_INET)
      return mimosa::StringRef((const char *)&in_, sizeof (in_));
    return mimosa::StringRef((const char *)&in6_, sizeof (in6_));
  }
}
