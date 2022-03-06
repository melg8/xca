#ifndef DIGEST_H
#define DIGEST_H

#include <openssl/evp.h>

#include <QString>

class digest {
 private:
  static int default_md;
  int md_nid;

 public:
  static const QList<int> all_digests;

  digest() = default;
  digest(int nid);
  digest(const EVP_MD* md);
  digest(const QString& name);
  digest(const digest& d) = default;
  digest& operator=(const digest& d) = default;

  bool isInsecure() const;
  const EVP_MD* MD() const;
  QString name() const;

  static void setDefault(const QString& def);
  static const digest getDefault();
};

#endif  // DIGEST_H
