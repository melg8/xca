#ifndef PKI_PKCS12_H
#define PKI_PKCS12_H

#include "pki_multi.h"

class pki_key;
class pki_x509;

class pki_pkcs12 : public pki_multi {
  Q_OBJECT
  friend class pki_x509;
  friend class pki_evp;

 protected:
  QString alias;
  pki_x509* cert;
  pki_key* key;

 public:
  pki_pkcs12(const QString& d, pki_x509* acert, pki_key* akey);
  pki_pkcs12(const QString& fname);

  pki_key* getKey() const { return key; }
  pki_x509* getCert() const { return cert; }
  void writePKCS12(XFile& file) const;
};

#endif  // PKI_PKCS12_H
