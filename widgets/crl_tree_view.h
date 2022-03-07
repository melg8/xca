#ifndef CRL_TREE_VIEW_H
#define CRL_TREE_VIEW_H

#include "lib/db_crl.h"
#include "xca_tree_view.h"

class pki_x509;

class CrlTreeView final : public XcaTreeView {
  Q_OBJECT

  [[nodiscard]] db_crl* crls() const {
    return dynamic_cast<db_crl*>(basemodel);
  }

 public:
  CrlTreeView(QWidget* parent) : XcaTreeView(parent) {}
  void showPki(pki_base* pki) final;
  ExportDialog* exportDialog(const QModelIndexList& index) final;

 public slots:
  void newItem(pki_x509* cert);
  void newItem();
  void load();
};

#endif  // CRL_TREE_VIEW_H
