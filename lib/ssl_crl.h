#ifndef SSL_CRL_H
#define SSL_CRL_H

typedef struct X509_crl_st X509_CRL;
typedef struct X509_name_st X509_NAME;
typedef struct evp_pkey_st EVP_PKEY;
typedef struct evp_md_st EVP_MD;

struct SslCrl {
    X509_CRL *crl_ = nullptr;

    SslCrl() noexcept;
    ~SslCrl() noexcept;

    void SetVersion2() noexcept;
    void SetIssuerName(const X509_NAME *issuer_name) noexcept;
    void SignSortedCrl(EVP_PKEY *pkey, const EVP_MD *md) noexcept;

    SslCrl& operator=(X509_CRL *lhs) noexcept;
};

#endif // SSL_CRL_H
