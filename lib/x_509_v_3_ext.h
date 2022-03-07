#ifndef X_509_V_3_EXT_H
#define X_509_V_3_EXT_H

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <QList>
#include <QStringList>

class QString;

class x509v3ext {
 private:
  X509_EXTENSION* ext;
  [[nodiscard]] const ASN1_OBJECT* object() const;

 public:
  x509v3ext();
  x509v3ext(const X509_EXTENSION* n);
  x509v3ext(const x509v3ext& n);
  ~x509v3ext();
  x509v3ext& set(const X509_EXTENSION* n);
  x509v3ext& create(int nid, const QString& et, X509V3_CTX* ctx = nullptr);
  x509v3ext& create_ia5(int nid, const QString& et, X509V3_CTX* ctx = nullptr);
  x509v3ext& operator=(const x509v3ext& x);
  // bool operator == (const x509v3ext &x) const;
  [[nodiscard]] QString getObject() const;
  [[nodiscard]] int getCritical() const;
  [[nodiscard]] ASN1_OCTET_STRING* getData() const;
  [[nodiscard]] QString getValue() const;
  [[nodiscard]] QString getHtmlValue() const;
  [[nodiscard]] QString getConsoleValue(const QString& indent) const;
  [[nodiscard]] QString getHtml() const;
  [[nodiscard]] QString getConsole(const QString& indent) const;
  [[nodiscard]] X509_EXTENSION* get() const;
  [[nodiscard]] bool isValid() const;
  [[nodiscard]] int nid() const;
  [[nodiscard]] void* d2i() const;
  bool genConf(QString* single, QString* adv) const;
  bool parse_generic(QString* single, QString* adv) const;

 protected:
  [[nodiscard]] QString parse_critical() const;
  bool parse_certpol(QString* single, QString* adv) const;
  bool parse_ainfo(QString* single, QString* adv) const;
  bool parse_Crldp(QString* single, QString* adv) const;
  bool parse_eku(QString* single, QString* adv) const;
  bool parse_generalName(QString* single, QString* adv) const;
  bool parse_ia5(QString* single, QString* adv) const;
  bool parse_bc(QString* single, QString* adv) const;
  bool parse_bitstring(QString* single, QString* adv) const;
  bool parse_sKeyId(QString* single, QString* adv) const;
  bool parse_aKeyId(QString* single, QString* adv) const;
  bool parse_inhibitAnyPolicy(QString*, QString* adv) const;
  bool parse_policyConstraints(QString*, QString* adv) const;
  bool parse_policyMappings(QString*, QString* adv) const;
  bool parse_nameConstraints(QString*, QString* adv) const;
};

class extList : public QList<x509v3ext> {
 public:
  void setStack(const STACK_OF(X509_EXTENSION) * st, int start = 0);
  STACK_OF(X509_EXTENSION) * getStack();
  [[nodiscard]] QString getHtml(const QString& sep) const;
  [[nodiscard]] QString getConsole(const QString& indent) const;
  bool delByNid(int nid);
  int delInvalid();
  int idxByNid(int nid);
  bool genConf(int nid, QString* single, QString* adv = nullptr);
  void genGenericConf(QString* adv);
  bool search(const QRegExp& pattern);
};

#endif  // X_509_V_3_EXT_H
