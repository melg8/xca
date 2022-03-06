#ifndef __BIOBYTEARRAY_H
#define __BIOBYTEARRAY_H

#include <QByteArray>
#include <QString>

#include <openssl/bio.h>

class BioByteArray {
 protected:
  BIO* read_write;
  BIO* read_only;
  QByteArray store;

  void set(const QByteArray& qba);
  void add(const QByteArray& qba);
  void biowrite(const QByteArray& qba);
  void cleanse_and_free(BIO* bio);

 public:
  BioByteArray(const QByteArray& qba)
      : read_write(nullptr), read_only(nullptr), store(qba) {}
  BioByteArray(const BioByteArray& bba)
      : read_write(nullptr), read_only(nullptr), store(bba.byteArray()) {}
  BioByteArray() : read_write(nullptr), read_only(nullptr), store() {}
  ~BioByteArray();
  int size() const;
  BIO* bio();
  BIO* ro();
  QByteArray byteArray() const;
  QString qstring() const;
  operator BIO*();
  operator QByteArray();
  BioByteArray& operator=(const BioByteArray& other);
  BioByteArray& operator=(const QByteArray& qba);
  BioByteArray& operator+=(const BioByteArray& other);
  BioByteArray& operator+=(const QByteArray& qba);
};

#endif
