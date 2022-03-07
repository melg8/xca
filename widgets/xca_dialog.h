#ifndef XCA_DIALOG_H
#define XCA_DIALOG_H

#include <QDialog>
#include "ui_xca_dialog.h"

#include "lib/pki_base.h"

class XcaDialog : public QDialog, public Ui::XcaDialog {
  QWidget* widg;

 public:
  XcaDialog(QWidget* parent,
            enum pki_type type,
            QWidget* w,
            const QString& t,
            const QString& desc,
            const QString& help_ctx = QString());
  void noSpacer();
  void aboutDialog(const QPixmap& left);
};

#endif  // XCA_DIALOG_H
