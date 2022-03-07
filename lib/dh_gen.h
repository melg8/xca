#ifndef DH_GEN_H
#define DH_GEN_H

#include <QThread>

#include "exception.h"

#include <QString>
#include <utility>

class DHgen final : public QThread {
  QString fname;
  int bits;
  errorEx err;

 public:
  DHgen(QString n, int b) : QThread(), fname(std::move(n)), bits(b) {}
  [[nodiscard]] QString filename() const { return fname; }
  [[nodiscard]] errorEx error() const { return err; }

 protected:
  void run() final;
};
#endif  // DH_GEN_H
