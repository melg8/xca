#ifndef PKI_X_509_SUPER_H
#define PKI_X_509_SUPER_H

#include "pki_base.h"

#include <openssl/pem.h>
#include <openssl/x509.h>
#include "digest.h"
#include "pki_key.h"
#include "x_509_name.h"
#include "x_509_v_3_ext.h"

#define VIEW_x509super_keyid 6

class pki_x509name : public pki_base {
 protected:
  [[nodiscard]] QByteArray PEM_comment() const override;

 public:
  pki_x509name(const QString& name = QString());
  pki_x509name(const pki_x509name* n);
  [[nodiscard]] virtual x509name getSubject() const = 0;
  void autoIntName(const QString& file) override;
  QVariant column_data(const dbheader* hd) const override;
  [[nodiscard]] bool visible() const override;
  void collect_properties(QMap<QString, QString>& prp) const override;
};

class pki_x509super : public pki_x509name {
  Q_OBJECT
 protected:
  QVariant keySqlId;
  [[nodiscard]] virtual int sigAlg() const = 0;
  void collect_properties(QMap<QString, QString>& prp) const override;

 public:
  pki_x509super(const QString& name = QString());
  pki_x509super(const pki_x509super* x);
  ~pki_x509super() override;
  [[nodiscard]] unsigned pubHash() const;
  [[nodiscard]] virtual pki_key* getPubKey() const = 0;
  [[nodiscard]] virtual extList getV3ext() const = 0;
  [[nodiscard]] virtual QString getSigAlg() const;
  [[nodiscard]] virtual const digest getDigest() const;
  QVariant getKeySqlId() { return keySqlId; }
  [[nodiscard]] pki_key* getRefKey() const;
  bool compareRefKey(pki_key* ref) const;
  void setRefKey(pki_key* ref);
  void delRefKey(pki_key* ref);
  QVariant column_data(const dbheader* hd) const override;
  void opensslConf(QString fname);
  [[nodiscard]] bool visible() const override;
  [[nodiscard]] bool hasPrivKey() const;
  QVariant getIcon(const dbheader* hd) const override;
  QSqlError lookupKey();
  QSqlError insertSqlData() override;
  QSqlError deleteSqlData() override;
  void restoreSql(const QSqlRecord& rec) override;
};

#endif  // PKI_X_509_SUPER_H
