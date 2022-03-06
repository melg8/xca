#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <QDialog>
#include "lib/base.h"

#include "MainWindow.h"
#include "SearchPkcs11.h"
#include "ui_Options.h"

class Options final : public QDialog, public Ui::Options {
  Q_OBJECT
 private:
  SearchPkcs11* searchP11;
  QStringList string_opts;
  QString getDnString(QListWidget* w);
  void setDnString(QString dn, QListWidget* w);

 public:
  Options(QWidget* parent);
  ~Options() final;
  int exec() final;

 public slots:
  void on_extDNadd_clicked();
  void on_extDNdel_clicked();
  void on_expDNadd_clicked();
  void on_expDNdel_clicked();
  void on_expDNdefault_clicked();
  void on_addButton_clicked(void);
  void on_removeButton_clicked(void);
  void on_searchPkcs11_clicked(void);
  void addLib(QString);
};

#endif
