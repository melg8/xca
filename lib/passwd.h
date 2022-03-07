#ifndef PASSWD_H
#define PASSWD_H

#include <QByteArray>

class Passwd : public QByteArray {
 public:
  void cleanse();

  Passwd() = default;
  ~Passwd();
  Passwd(const Passwd& lhs) = default;
  Passwd& operator=(const Passwd& lhs) = default;

  [[nodiscard]] const unsigned char* constUchar() const;
  Passwd& operator=(const char* p) { return (Passwd&)QByteArray::operator=(p); }
  Passwd& operator=(const QByteArray& other) {
    return (Passwd&)QByteArray::operator=(other);
  }
};

#endif  // PASSWD_H
