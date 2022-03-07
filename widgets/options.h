#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include "lib/base.h"

#include "main_window.h"
#include "search_pkcs_11.h"
#include "ui_options.h"

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
  void on_addButton_clicked();
  void on_removeButton_clicked();
  void on_searchPkcs11_clicked();
  void addLib(QString);
};

#endif  // OPTIONS_H
