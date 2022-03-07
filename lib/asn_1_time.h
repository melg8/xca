#ifndef ASN_1_TIME_H
#define ASN_1_TIME_H

#include <QDateTime>

#include <openssl/asn1.h>

#include <QString>

#define SECS_PER_MINUTE (60)
#define SECS_PER_HOUR (SECS_PER_MINUTE * 60)
#define SECS_PER_DAY (SECS_PER_HOUR * 24)

#define MSECS_PER_MINUTE (SECS_PER_MINUTE * 1000)
#define MSECS_PER_HOUR (SECS_PER_HOUR * 1000)

class a1time : public QDateTime {
 private:
  ASN1_TIME* atime;
  int from_asn1(const ASN1_TIME* a);
  int set_asn1(const QString& str, int type);

 public:
  a1time();
  a1time(const QDateTime& a);
  a1time(const ASN1_TIME* a);
  a1time(const a1time& a);
  a1time(const QString& plain);
  a1time& operator=(const a1time& a);
  ~a1time();
  a1time& set(const ASN1_TIME* a);
  int fromPlain(const QString& plain);
  a1time& setUndefined();
  [[nodiscard]] bool isUndefined() const;
  [[nodiscard]] QString toString(QString fmt,
                                 Qt::TimeSpec spec = Qt::UTC) const;
  [[nodiscard]] QString toPretty() const;
  [[nodiscard]] QString toPrettyGMT() const;
  [[nodiscard]] QString toPlain(const QString& fmt = QString()) const;
  [[nodiscard]] QString toPlainUTC() const;
  [[nodiscard]] QString toSortable() const;
  [[nodiscard]] QString toFancy() const;
  [[nodiscard]] QString isoLocalDate() const;
  ASN1_TIME* get();
  ASN1_TIME* get_utc();
  static QDateTime now(int delta = 0);
  QByteArray i2d();
  void d2i(QByteArray& ba);
  [[nodiscard]] qint64 age() const;
};

#endif  // ASN_1_TIME_H
