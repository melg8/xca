#ifndef REVOCATION_LIST_H
#define REVOCATION_LIST_H

#include <QDialog>
#include "ui_RevocationList.h"

#include <QModelIndex>
#include "lib/x_509_rev.h"
#include "ui_Revoke.h"

class pki_x509;

class RevocationList : public QDialog, public Ui::RevocationList {
  Q_OBJECT

 private:
  x509revList revList;
  pki_x509* issuer;

 public:
  static void setupRevocationView(QTreeWidget* certsWidget,
                                  const x509revList& revList,
                                  const pki_x509* iss);
  RevocationList(QWidget* w = nullptr);
  void setRevList(const x509revList& rl, pki_x509* issuer);
  const x509revList& getRevList();

 public slots:
  void on_addRev_clicked();
  void on_delRev_clicked();
  void on_editRev_clicked();
  void gencrl();
  void on_certList_itemDoubleClicked(QTreeWidgetItem*);
};

class Revocation : public QDialog, public Ui::Revoke {
  Q_OBJECT

 public:
  Revocation(QModelIndexList indexes, QWidget* w = nullptr);
  x509rev getRevocation();
  void setRevocation(x509rev r);
};

#endif  // REVOCATION_LIST_H
