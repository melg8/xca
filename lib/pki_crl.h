#ifndef PKI_CRL_H
#define PKI_CRL_H

#include <openssl/bio.h>

#include "crl_job_settings.h"
#include "pki_x_509.h"
#include "x_509_name.h"

#define VIEW_crls_num 6
#define VIEW_crls_issuer 7
#define VIEW_crls_crl 8

#include "digest.h"

[[nodiscard]] static CrlJobSettings CrlJobSettingsFrom(
    pki_x509* issuer) noexcept {
  return CrlJobSettings{true,
                        true,
                        true,
                        issuer->getCrlNumber().getLong() > 0,
                        a1int(issuer->getCrlNumber().getLong() + 1UL),
                        digest::getDefault(),
                        a1time{},
                        a1time{}.addDays(issuer->getCrlDays())};
}

class crljob {
 public:
  pki_x509* issuer;
  CrlJobSettings settings;
  int crlDays;

  crljob(pki_x509* x)
      : issuer(x),
        settings(CrlJobSettingsFrom(issuer)),
        crlDays(issuer->getCrlDays()) {}
  crljob() = delete;
};

class pki_crl final : public pki_x509name {
  Q_OBJECT
  friend class pki_x509;

 protected:
  QVariant issuerSqlId;
  X509_CRL* crl;
  [[nodiscard]] extList extensions() const;
  void collect_properties(QMap<QString, QString>& prp) const final;

 public:
  pki_crl(const QString name = "");
  ~pki_crl() final;
  void fromPEM_BIO(BIO* bio, const QString& name) final;
  void fload(const QString& fname) final;
  [[nodiscard]] QString getSigAlg() const;
  void writeDefault(const QString& dirname) const final;
  void createCrl(const QString d, pki_x509* iss);
  void addRev(const x509rev& rev, bool withReason = true);
  void addExt(int nid, QString value);
  void addV3ext(const x509v3ext& e);
  void sign(pki_key* key, const digest& digest);
  void writeCrl(XFile& file, bool pem = true) const;
  [[nodiscard]] pki_x509* getIssuer() const;
  [[nodiscard]] QString getIssuerName() const;
  void setIssuer(pki_x509* iss);
  [[nodiscard]] x509name getSubject() const final;
  void setLastUpdate(const a1time& t);
  void setNextUpdate(const a1time& t);
  [[nodiscard]] a1time getNextUpdate() const;
  [[nodiscard]] a1time getLastUpdate() const;
  bool verify(pki_x509* issuer);
  [[nodiscard]] int numRev() const;
  x509revList getRevList();
  QString printV3ext();
  x509v3ext getExtByNid(int nid);
  long getVersion();
  QVariant column_data(const dbheader* hd) const final;
  QVariant getIcon(const dbheader* hd) const final;
  a1time column_a1time(const dbheader* hd) const final;
  [[nodiscard]] QString getMsg(msg_type msg) const final;
  void d2i(QByteArray& ba);
  [[nodiscard]] QByteArray i2d() const final;
  void setCrlNumber(a1int num);
  bool getCrlNumber(a1int* num) const;
  [[nodiscard]] a1int getCrlNumber() const;
  bool pem(BioByteArray&) final;
  [[nodiscard]] bool visible() const final;
  QSqlError lookupIssuer();
  QSqlError insertSqlData() final;
  QSqlError deleteSqlData() final;
  void restoreSql(const QSqlRecord& rec) final;
  [[nodiscard]] QStringList icsVEVENT() const;
  void print(BioByteArray& b, enum print_opt opt) const final;
};

#endif  // PKI_CRL_H
