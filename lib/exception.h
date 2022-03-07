#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QObject>
#include <QSqlError>
#include <QString>

#include "base.h"

enum open_result { pw_cancel, pw_ok, pw_exit, open_abort };

class errorEx {
 protected:
  QString msg;

 public:
  errorEx(QString txt = "", QString className = "") {
    msg = txt;
    if (!className.isEmpty()) msg += " (" + className + ")";
  }
  errorEx(const QSqlError& e) { msg = e.text(); }
  void appendString(QString s) { msg = msg + " " + s; }
  [[nodiscard]] QString getString() const { return msg; }
  [[nodiscard]] const char* getCString() const { return msg.toLatin1(); }
  [[nodiscard]] bool isEmpty() const { return msg.isEmpty(); }
};

#endif  // EXCEPTION_H
