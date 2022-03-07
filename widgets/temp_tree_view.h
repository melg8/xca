#ifndef TEMP_TREE_VIEW_H
#define TEMP_TREE_VIEW_H

#include "xca_tree_view.h"

#include "lib/db_temp.h"

class TempTreeView final : public XcaTreeView {
  Q_OBJECT

  [[nodiscard]] db_temp* temps() const {
    return dynamic_cast<db_temp*>(basemodel);
  }

  bool runTempDlg(pki_temp* temp);

 public:
  TempTreeView(QWidget* parent) : XcaTreeView(parent) {}
  void fillContextMenu(QMenu* menu,
                       QMenu* subExport,
                       const QModelIndex& index,
                       QModelIndexList indexes) final;
  void showPki(pki_base* pki) final;
  bool alterTemp(pki_temp* temp);
  ExportDialog* exportDialog(const QModelIndexList& index) final;

 public slots:
  void certFromTemp();
  void reqFromTemp();
  void duplicateTemp();
  void newItem();
  void load();

 signals:
  void newReq(pki_temp*);
  void newCert(pki_temp*);
};

#endif  // TEMP_TREE_VIEW_H
