#include "pki_pkcs_7.h"

#include "exception.h"
#include "func.h"
#include "pki_key.h"
#include "pki_x_509.h"

#include <openssl/x509.h>

pki_pkcs7::pki_pkcs7(const QString& name) : pki_multi(name) { p7 = nullptr; }

pki_pkcs7::~pki_pkcs7() {
  if (p7) PKCS7_free(p7);
}

void pki_pkcs7::encryptFile(pki_x509* crt, const QString& name) {
  XFile f(name);
  encryptBio(crt, BioByteArray(f.readAll()).ro());
}

void pki_pkcs7::encryptBio(pki_x509* crt, BIO* bio) {
  STACK_OF(X509) * certstack;
  if (!crt) return;
  certstack = sk_X509_new_null();
  sk_X509_push(certstack, crt->getCert());
  openssl_error();
  if (p7) PKCS7_free(p7);
  p7 = PKCS7_encrypt(certstack, bio, EVP_des_ede3_cbc(), PKCS7_BINARY);
  openssl_error();
  sk_X509_free(certstack);
}

void pki_pkcs7::signBio(pki_x509* crt, BIO* bio) {
  pki_key* privkey;
  EVP_PKEY* pk;
  STACK_OF(X509) * certstack;
  if (!crt) return;
  privkey = crt->getRefKey();
  if (!privkey)
    throw errorEx("No private key for signing found", getClassName());
  certstack = sk_X509_new_null();

  pki_x509* signer = crt->getSigner();
  if (signer == crt) signer = nullptr;
  while (signer != nullptr) {
    sk_X509_push(certstack, signer->getCert());
    openssl_error();
    if (signer == signer->getSigner())
      signer = nullptr;
    else
      signer = signer->getSigner();
  }
  if (p7) PKCS7_free(p7);
  pk = privkey->decryptKey();
  p7 = PKCS7_sign(crt->getCert(), pk, certstack, bio, PKCS7_BINARY);
  EVP_PKEY_free(pk);
  openssl_error();
  sk_X509_free(certstack);
}

void pki_pkcs7::signFile(pki_x509* crt, const QString& name) {
  XFile f(name);
  f.open_read();
  if (crt) signBio(crt, BioByteArray(f.readAll()).ro());
}

void pki_pkcs7::signCert(pki_x509* crt, pki_x509* contCert) {
  BioByteArray bba;
  i2d_X509_bio(bba, contCert->getCert());
  signBio(crt, bba);
  openssl_error();
}

void pki_pkcs7::writeP7(XFile& file, bool PEM) {
  if (!p7) {
    p7 = PKCS7_new();
    PKCS7_set_type(p7, NID_pkcs7_signed);
    PKCS7_content_new(p7, NID_pkcs7_data);
    pki_openssl_error();
  }
  foreach (pki_base* pki, multi) {
    pki_x509* x = dynamic_cast<pki_x509*>(pki);
    if (x) PKCS7_add_certificate(p7, X509_dup(x->getCert()));
  }
  BioByteArray b;
  if (PEM)
    PEM_write_bio_PKCS7(b, p7);
  else
    i2d_PKCS7_bio(b, p7);
  openssl_error();
  file.write(b);
}

void pki_pkcs7::append_certs(PKCS7* myp7, const QString& name) {
  STACK_OF(X509)* certstack = nullptr;

  pki_openssl_error();

  if (myp7 == nullptr) return;

  setFilename(name);
  autoIntName(name);

  switch (OBJ_obj2nid(myp7->type)) {
    case NID_pkcs7_signed:
      certstack = myp7->d.sign->cert;
      myp7->d.sign->cert = nullptr;
      break;
    case NID_pkcs7_signedAndEnveloped:
      certstack = myp7->d.signed_and_enveloped->cert;
      myp7->d.signed_and_enveloped->cert = nullptr;
      break;
  }
  if (!certstack) return;

  for (int x = 0; x < sk_X509_num(certstack); x++) {
    X509* c = X509_dup(sk_X509_value(certstack, x));
    pki_x509* cert = new pki_x509(c);
    openssl_error();
    cert->autoIntName(getIntName());
    cert->pkiSource = imported;
    inheritFilename(cert);
    append_item(cert);
  }
  sk_X509_free(certstack);
  PKCS7_free(myp7);
}

void pki_pkcs7::fromPEM_BIO(BIO* bio, const QString& name) {
  PKCS7* myp7 = PEM_read_bio_PKCS7(bio, nullptr, nullptr, nullptr);
  append_certs(myp7, name);
}

void pki_pkcs7::fromPEMbyteArray(const QByteArray& ba, const QString& name) {
  // Skip the pki_multi::fromPEMbyteArray(),
  // which would cause an endless loop
  pki_base::fromPEMbyteArray(ba, name);
}

void pki_pkcs7::fload(const QString& name) {
  PKCS7* myp7;
  XFile file(name);
  file.open_read();
  QByteArray ba(file.readAll());

  myp7 = PEM_read_bio_PKCS7(BioByteArray(ba).ro(), nullptr, nullptr, nullptr);
  if (!myp7) {
    ign_openssl_error();
    myp7 = d2i_PKCS7_bio(BioByteArray(ba).ro(), nullptr);
  }
  if (ign_openssl_error()) {
    if (myp7) PKCS7_free(myp7);
    throw errorEx(
        tr("Unable to load the PKCS#7 file %1. Tried PEM and DER format.")
            .arg(name));
  }
  append_certs(myp7, name);
}