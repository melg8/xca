#ifndef PKI_PKCS7_H
#define PKI_PKCS7_H

#include <QString>
#include "pki_multi.h"

#include <openssl/pkcs7.h>

class pki_x509;

class pki_pkcs7 final : public pki_multi {
  Q_OBJECT

  friend class pki_x509;

 protected:
  PKCS7* p7;
  void signBio(pki_x509* crt, BIO* bio);
  void encryptBio(pki_x509* crt, BIO* bio);
  void append_certs(PKCS7* myp7, const QString& name);

 public:
  pki_pkcs7(const QString& name = QString());
  ~pki_pkcs7() final;

  void signFile(pki_x509* crt, const QString& name);
  void signCert(pki_x509* crt, pki_x509* contCert);
  void encryptFile(pki_x509* crt, const QString& name);
  void writeP7(XFile& file, bool PEM);
  void fromPEM_BIO(BIO* bio, const QString& name) final;
  void fromPEMbyteArray(const QByteArray&, const QString&) final;
  void fload(const QString& name) final;
};

#endif  // PKI_PKCS7_H
