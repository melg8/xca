#ifndef REQ_TREE_VIEW_H
#define REQ_TREE_VIEW_H

#include "x_509_super_tree_view.h"

#include "lib/db_x_509_req.h"

class ReqTreeView final : public X509SuperTreeView {
  Q_OBJECT

  db_x509req* reqs() const { return dynamic_cast<db_x509req*>(basemodel); }

 public:
  ReqTreeView(QWidget* parent) : X509SuperTreeView(parent) {}
  void fillContextMenu(QMenu* menu,
                       QMenu* subExport,
                       const QModelIndex& index,
                       QModelIndexList indexes) final;
  ExportDialog* exportDialog(const QModelIndexList& indexes) final;

 public slots:
  void toRequest();
  void signReq();
  void markSigned();
  void unmarkSigned();
  void load();
};

#endif  // REQ_TREE_VIEW_H
