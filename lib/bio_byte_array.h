#ifndef BIO_BYTE_ARRAY_H
#define BIO_BYTE_ARRAY_H

#include <QByteArray>
#include <QString>
#include <utility>

#include <openssl/bio.h>

class BioByteArray {
 protected:
  BIO* read_write{nullptr};
  BIO* read_only{nullptr};
  QByteArray store{};

  void set(const QByteArray& qba);
  void add(const QByteArray& qba);
  void biowrite(const QByteArray& qba);
  void cleanse_and_free(BIO* bio);

 public:
  BioByteArray(QByteArray qba) : store(std::move(qba)) {}
  BioByteArray(const BioByteArray& bba) : store(bba.byteArray()) {}
  BioByteArray() = default;
  ~BioByteArray();
  [[nodiscard]] int size() const;
  BIO* bio();
  BIO* ro();
  [[nodiscard]] QByteArray byteArray() const;
  [[nodiscard]] QString qstring() const;
  operator BIO*();
  operator QByteArray();
  BioByteArray& operator=(const BioByteArray& other);
  BioByteArray& operator=(const QByteArray& qba);
  BioByteArray& operator+=(const BioByteArray& other);
  BioByteArray& operator+=(const QByteArray& qba);
};

#endif  // BIO_BYTE_ARRAY_H
