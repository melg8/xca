#ifndef PKI_X_509_REQ_H
#define PKI_X_509_REQ_H

#include <openssl/pem.h>
#include <openssl/x509.h>
#include "digest.h"
#include "pki_key.h"
#include "pki_x509super.h"
#include "x509name.h"
#include "x509v3ext.h"

#define VIEW_x509req_request 7
#define VIEW_x509req_signed 8

class pki_x509;

class pki_x509req final : public pki_x509super {
  Q_OBJECT

  mutable int x509count;

 protected:
  X509_REQ* request;
  bool done;
  int sigAlg() const final;
  void collect_properties(QMap<QString, QString>& prp) const final;

 public:
  pki_x509req(const QString& name = QString());
  pki_x509req(const pki_x509req* req);
  ~pki_x509req() final;

  extList getV3ext() const final;
  void fromPEM_BIO(BIO* bio, const QString& name) final;
  void fload(const QString& fname) final;
  void writeDefault(const QString& dirname) const final;
  x509name getSubject() const final;
  void writeReq(XFile& file, bool pem) const;
  void markSigned(bool signe);
  void print(BioByteArray& b, enum print_opt opt) const final;
  X509_REQ* getReq() { return request; }
  void addAttribute(int nid, QString content);
  QString getAttribute(int nid) const;
  int issuedCerts() const;

  bool verify() const;
  pki_key* getPubKey() const final;
  void createReq(pki_key* key,
                 const x509name& dn,
                 const digest& digest,
                 extList el);
  void setSubject(const x509name& n);
  QVariant column_data(const dbheader* hd) const final;
  QVariant getIcon(const dbheader* hd) const final;
  void setDone(bool d = true) { done = d; }
  bool getDone() const { return done; }
  void resetX509count() const { x509count = -1; }
  virtual QString getMsg(msg_type msg) const final;
  void d2i(QByteArray& ba);
  QByteArray i2d() const final;
  bool pem(BioByteArray&) final;
  bool visible() const final;
  QSqlError insertSqlData() final;
  QSqlError deleteSqlData() final;
  void restoreSql(const QSqlRecord& rec) final;
};

Q_DECLARE_METATYPE(pki_x509req*)

#endif  // PKI_X_509_REQ_H
