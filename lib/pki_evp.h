#ifndef PKI_EVP_H
#define PKI_EVP_H

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <QProgressBar>
#include <QString>
#include "Passwd.h"
#include "pki_key.h"

#define VIEW_private_ownpass 9

class pass_info;

class pki_evp final : public pki_key {
  Q_OBJECT
  QByteArray encKey;
  void init();
  QByteArray getEncKey() const;
  QString encKey_b64() { return QString::fromLatin1(encKey.toBase64()); }
  static QString _sha512passwd(QByteArray pass,
                               QString salt,
                               int size,
                               int repeat);
  void set_EVP_PKEY(EVP_PKEY* pkey, QString name = QString());

 protected:
  bool openssl_pw_error() const;

 public:
  static QString passHash;
  static Passwd passwd;
  static QString md5passwd(QByteArray pass);
  static QString sha512passwd(QByteArray pass, QString salt);
  static QString sha512passwT(QByteArray pass, QString salt);

  pki_evp(const QString& n = QString(), int type = EVP_PKEY_RSA);
  pki_evp(const pki_evp* pkey);
  pki_evp(EVP_PKEY* pkey);
  ~pki_evp() final;

  void generate(const keyjob& task) final;
  void setOwnPass(enum passType);
  void set_evp_key(EVP_PKEY* pkey);
  void encryptKey(const char* password = nullptr);
  void bogusEncryptKey();
  EVP_PKEY* decryptKey() const final;
  EVP_PKEY* priv2pub(EVP_PKEY* privateKey);
  static QString removeTypeFromIntName(QString n);
  void fromPEMbyteArray(const QByteArray& ba, const QString& name) final;
  void fload(const QString& fname) final;
  bool pem(BioByteArray&) final;
  EVP_PKEY* load_ssh_ed25519_privatekey(const QByteArray& ba,
                                        const pass_info& p);
  void writeDefault(const QString& dirname) const final;
  void writeKey(XFile& file,
                const EVP_CIPHER* enc,
                pem_password_cb* cb,
                bool pem) const;
  void writePKCS8(XFile& file,
                  const EVP_CIPHER* enc,
                  pem_password_cb* cb,
                  bool pem) const;
  void writePVKprivate(XFile& file, pem_password_cb* cb) const;
  bool verify_priv(EVP_PKEY* pkey) const final;
  QVariant getIcon(const dbheader* hd) const final;
  bool sqlUpdatePrivateKey();
  QSqlError insertSqlData() final;
  QSqlError deleteSqlData() final;
  void restoreSql(const QSqlRecord& rec) final;
};

#endif  // PKI_EVP_H
