#ifndef ASN_1_INT_H
#define ASN_1_INT_H

#include <QString>

#include <memory>

using ASN1_INTEGER = struct asn1_string_st;

struct Asn1IntegerDeleter {
  void operator()(ASN1_INTEGER* ptr);
};

using Asn1Integer = std::unique_ptr<ASN1_INTEGER, Asn1IntegerDeleter>;

class a1int {
 private:
  Asn1Integer in;

  static ASN1_INTEGER* dup(const ASN1_INTEGER* a);
  a1int& setQString(const QString& s, int dec);
  [[nodiscard]] QString toQString(int dec) const;

 public:
  a1int();
  a1int(const ASN1_INTEGER* i);
  a1int(const a1int& a);
  a1int(long l);
  a1int(const QString& hex);
  ~a1int();

  [[nodiscard]] QString toHex() const;
  [[nodiscard]] QString toDec() const;
  [[nodiscard]] long getLong() const;
  [[nodiscard]] ASN1_INTEGER* get() const;
  [[nodiscard]] const ASN1_INTEGER* get0() const;
  [[nodiscard]] int derSize() const;

  a1int& set(const ASN1_INTEGER* i);
  a1int& set(long l);
  a1int& setHex(const QString& s);
  a1int& setDec(const QString& s);
  a1int& setRaw(const unsigned char* data, unsigned len);
  QByteArray i2d();

  a1int& operator++();
  a1int operator++(int);
  a1int& operator=(const a1int& a);
  a1int& operator=(long i);
  bool operator>(const a1int& a) const;
  bool operator<(const a1int& a) const;
  bool operator==(const a1int& a) const;
  bool operator!=(const a1int& a) const;
  //  operator QString() const;
};

#endif  // ASN_1_INT_H
