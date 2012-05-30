namespace hefur
{
  inline mimosa::StringRef
  Address::key() const
  {
    if (family_ == AF_INET)
      return mimosa::StringRef((const char *)&in_, sizeof (in_));
    return mimosa::StringRef((const char *)&in6_, sizeof (in6_));
  }

  inline
  Address::Address()
  {
    ::memset(this, 0, sizeof (*this));
  }

  inline
  Address::Address(const Address & a)
  {
    ::memcpy(this, &a, sizeof (a));
  }

  inline Address &
  Address::operator=(const Address & a)
  {
    ::memcpy(this, &a, sizeof (a));
    return *this;
  }

  inline bool
  Address::operator==(const Address & other) const
  {
    if (family_ != other.family_)
      return false;

    switch (family_)
    {
    case AF_INET:
      return !::memcmp(&in_, &other.in_, sizeof (in_));

    case AF_INET6:
      return !::memcmp(&in6_, &other.in6_, sizeof (in6_));

    default:
      return false;
    }
  }
}
