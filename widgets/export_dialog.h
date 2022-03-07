#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include <QDialog>
#include "ui_export_dialog.h"

#include "lib/pki_export.h"

#include <QModelIndexList>

class QPixmap;
class pki_base;

class ExportDialog final : public QDialog, public Ui::ExportDialog {
  Q_OBJECT

 protected:
  QString filter;

 public:
  ExportDialog(QWidget* w,
               const QString& title,
               const QString& filt,
               const QModelIndexList& indexes,
               const QPixmap& img,
               QList<const pki_export*> types,
               const QString& help_ctx = QString());
  ~ExportDialog() final;
  static bool mayWriteFile(const QString& fname);
  const pki_export* export_type(int idx = -1) const;

 public slots:
  void on_fileBut_clicked();
  void on_exportFormat_activated(int);
  void on_exportFormat_highlighted(int index);
  void accept() final;
};

#endif  // EXPORT_DIALOG_H
