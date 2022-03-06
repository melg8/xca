#ifndef __DHGEN_H
#define __DHGEN_H

#include <QThread>

#include "exception.h"

#include <QString>

class DHgen final : public QThread {
  QString fname;
  int bits;
  errorEx err;

 public:
  DHgen(const QString& n, int b) : QThread(), fname(n), bits(b) {}
  QString filename() const { return fname; }
  errorEx error() const { return err; }

 protected:
  void run() final;
};
#endif
