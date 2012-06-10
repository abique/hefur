#ifndef HEFUR_CRYPTO_SUPPORT_HH
# define HEFUR_CRYPTO_SUPPORT_HH

namespace hefur
{
  /**
   * This reflect the peer's crypto support. It may be sent by the peer
   * in the hope that we connect him or tell him which peers supports
   * crypto as the crypto handshake is not compatible with the plain
   * handshake. Though it is easily workable for the client to start
   * with a crypto handshake and then retry with a plain handshake,
   * so we can avoid the crypto support attribute.
   */
  enum CryptoSupport
  {
    kNotSupported,
    kSupported,
    kRequired,
  };
}

#endif /* !HEFUR_CRYPTO_SUPPORT_HH */
