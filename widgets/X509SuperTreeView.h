#ifndef X_509_SUPER_TREE_VIEW_H
#define X_509_SUPER_TREE_VIEW_H

#include "XcaTreeView.h"

#include "lib/db_x_509_req.h"

class X509SuperTreeView : public XcaTreeView {
  Q_OBJECT

  db_x509super* x509super() const {
    return dynamic_cast<db_x509super*>(basemodel);
  }

 protected:
  QMenu* transform;

 public:
  X509SuperTreeView(QWidget* parent) : XcaTreeView(parent) {
    transform = nullptr;
  }
  void fillContextMenu(QMenu* menu,
                       QMenu* subExport,
                       const QModelIndex& index,
                       QModelIndexList indexes) override;

 public slots:
  void showPki(pki_base* pki) override;
  void extractPubkey();
  void toTemplate();
  void toOpenssl();
};

#endif  // X_509_SUPER_TREE_VIEW_H
