#ifndef KEY_TREE_VIEW_H
#define KEY_TREE_VIEW_H

#include "xca_tree_view.h"

#include "lib/db_key.h"

class QAction;
class KeyTreeView final : public XcaTreeView {
  Q_OBJECT

  [[nodiscard]] db_key* keys() const {
    return dynamic_cast<db_key*>(basemodel);
  }

 public:
  KeyTreeView(QWidget* parent) : XcaTreeView(parent) {}
  void fillContextMenu(QMenu* menu,
                       QMenu* subExport,
                       const QModelIndex& index,
                       QModelIndexList indexes) final;
  void showPki(pki_base* pki) final;
  ExportDialog* exportDialog(const QModelIndexList& indexes) final;

 public slots:
  void resetOwnPass();
  void setOwnPass();
  void changePin();
  void initPin();
  void changeSoPin();
  void toToken();
  void newItem();
  void load();
  void newItem(const QString& name);
  void clipboardFormat(QAction*);
};

#endif  // KEY_TREE_VIEW_H
