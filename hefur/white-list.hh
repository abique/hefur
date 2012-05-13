#ifndef HEFUR_WHITE_LIST_HH
# define HEFUR_WHITE_LIST_HH

# include "info-sha1.hh"

namespace hefur
{
  class WhiteList
  {
  public:
    inline virtual ~WhiteList() {}

    virtual bool isAllowed(const InfoSha1 & info_sha1) const = 0;
  };
}

#endif /* !HEFUR_WHITE_LIST_HH */
