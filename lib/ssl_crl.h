#ifndef SSL_CRL_H
#define SSL_CRL_H

typedef struct X509_crl_st X509_CRL;

struct SslCrl {
    X509_CRL *crl_ = nullptr;
};

#endif // SSL_CRL_H
