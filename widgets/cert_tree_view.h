#ifndef CERT_TREE_VIEW_H
#define CERT_TREE_VIEW_H

#include "lib/db_x_509.h"
#include "x_509_super_tree_view.h"

class CertTreeView final : public X509SuperTreeView {
  Q_OBJECT

  [[nodiscard]] db_x509* certs() const {
    return dynamic_cast<db_x509*>(basemodel);
  }

 public:
  CertTreeView(QWidget* parent) : X509SuperTreeView(parent) {}
  void fillContextMenu(QMenu* menu,
                       QMenu* subExport,
                       const QModelIndex& index,
                       QModelIndexList indexes) final;
  ExportDialog* exportDialog(const QModelIndexList& index) final;

 public slots:
  void changeView();
  void toRequest();
  void toCertificate();
  void toToken();
  void toOtherToken();
  void genCrl();
  void loadPKCS7();
  void loadPKCS12();
  void deleteFromToken();
  void manageRevocations();
  void certRenewal();
  void caProperties();
  void revoke();
  void unRevoke();
  void load();
};

#endif  // CERT_TREE_VIEW_H
