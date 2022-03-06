#include "passwd.h"

#include <QByteArray>

void Passwd::cleanse() {
  memset(data(), 0, size());
  clear();
}

Passwd::~Passwd() { Passwd::cleanse(); }
const unsigned char* Passwd::constUchar() const {
  return reinterpret_cast<const unsigned char*>(size() ? constData() : "");
}
