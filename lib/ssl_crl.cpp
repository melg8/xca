#include "ssl_crl.h"

#include <openssl/x509.h>

SslCrl::SslCrl() noexcept : crl_{X509_CRL_new()}  {}

SslCrl::~SslCrl() noexcept {
    X509_CRL_free(crl_);
    crl_ = nullptr;
}

void SslCrl::SetVersion2() noexcept {
    X509_CRL_set_version(crl_, 1); /* version 2 CRL */
}

void SslCrl::SetIssuerName(const X509_NAME *issuer_name) noexcept {
    X509_CRL_set_issuer_name(crl_, const_cast<X509_NAME*>(issuer_name));
}

void SslCrl::SignSortedCrl(EVP_PKEY *pkey, const EVP_MD *md) noexcept {
    X509_CRL_sort(crl_);
    X509_CRL_sign(crl_, pkey, md);
}

SslCrl &SslCrl::operator=(X509_CRL *lhs) noexcept {
    if (lhs) {
        X509_CRL_free(crl_);
        crl_ = lhs;
    }
    return *this;
}
