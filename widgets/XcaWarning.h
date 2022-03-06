#ifndef XCA_WARNING_H
#define XCA_WARNING_H

#include <QMessageBox>

#include "lib/XcaWarningCore.h"

class xcaWarningBox : public QMessageBox {
 public:
  xcaWarningBox(QWidget* w,
                const QString& txt,
                QMessageBox::Icon icn = QMessageBox::Warning);
  void addButton(QMessageBox::StandardButton button,
                 const QString& text = QString());
};

class xcaWarningGui final : public QObject, public xcaWarning_i {
  Q_OBJECT

  int showBox(const QString& txt,
              QMessageBox::Icon icn,
              QMessageBox::StandardButtons b);

 public:
  void information(const QString& msg) final;
  void warning(const QString& msg) final;
  void warningv3(const QString& msg, const extList& el) final;
  bool yesno(const QString& msg) final;
  bool okcancel(const QString& msg) final;
  void sqlerror(QSqlError err) final;
  void error(const QString& msg) final;
};

#endif  // XCA_WARNING_H
