#ifndef X_FILE_H
#define X_FILE_H

#define _CRT_SECURE_NO_WARNINGS

#include <sys/stat.h>
#include <sys/types.h>
#include <QDebug>
#include <QFile>

#include <cerrno>
#include "exception.h"

#include <openssl/bio.h>

class XFile final : public QFile {
  Q_OBJECT

 public:
  bool open(OpenMode flags) final {
    bool o = QFile::open(flags | Unbuffered);
    if (error()) {
      throw errorEx(tr("Error opening file: '%1': %2")
                        .arg(fileName())
                        .arg(strerror(errno)));
    }
    return o;
  }
  XFile(const QString& name) : QFile(name) {}
  void retry_read() {
    seek(0);
    if (error()) {
      throw errorEx(tr("Error rewinding file: '%1': %2")
                        .arg(fileName())
                        .arg(strerror(errno)));
    }
  }
  bool open_key() {
    mode_t m = umask(077);
    bool o = open_write();
    umask(m);
    return o;
  }
  bool open_write() { return open(ReadWrite | Truncate); }
  bool open_read() { return open(ReadOnly); }
};

#endif  // X_FILE_H
