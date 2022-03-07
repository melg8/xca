#ifndef X_509_NAME_H
#define X_509_NAME_H

#include <openssl/x509.h>
#include <QString>
#include <QStringList>

class x509name {
 private:
  X509_NAME* xn;

 public:
  x509name();
  x509name(const X509_NAME* n);
  x509name(const x509name& n);
  x509name(STACK_OF(X509_NAME_ENTRY) * entries);
  ~x509name();
  x509name& set(const X509_NAME* n);
  x509name& set(const STACK_OF(X509_NAME_ENTRY) * entries);
  [[nodiscard]] QString oneLine(unsigned long flags = XN_FLAG_ONELINE) const;
  [[nodiscard]] int nid(int i) const;
  [[nodiscard]] QString getOid(int i) const;
  [[nodiscard]] QByteArray i2d() const;
  void d2i(QByteArray& ba);
  [[nodiscard]] QStringList entryList(int i) const;
  [[nodiscard]] QString getEntryByNid(int nid) const;
  [[nodiscard]] QString getEntry(int i) const;
  [[nodiscard]] QString getEntryTag(int i) const;
  [[nodiscard]] int entryCount() const;
  x509name& operator=(const x509name& x);
  bool operator==(const x509name& x) const;
  bool operator!=(const x509name& x) const;
  static int getNidByName(const QString& nid_name);
  void addEntryByNid(int nid, const QString entry);
  [[nodiscard]] QString checkLength() const;
  QString popEntryByNid(int nid);
  [[nodiscard]] X509_NAME* get() const;
  [[nodiscard]] const X509_NAME* get0() const;
  [[nodiscard]] QString getMostPopular() const;
  [[nodiscard]] QString taggedValues() const;
  [[nodiscard]] QString hash() const;
  [[nodiscard]] unsigned hashNum() const;
  [[nodiscard]] bool search(const QRegExp& pattern) const;
};

#endif  // X_509_NAME_H
