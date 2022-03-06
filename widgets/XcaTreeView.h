#ifndef XCA_TREE_VIEW_H
#define XCA_TREE_VIEW_H

#include <QTreeView>

#include <QHeaderView>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QTimer>

class database_model;
class MainWindow;
class db_base;
class dbheader;
class pki_base;
class QKeyEvent;
class QContextMenuEvent;
class QMenu;
class load_base;
class ExportDialog;

class XcaTreeView : public QTreeView {
  Q_OBJECT

  dbheader* curr_hd;
  QTimer throttle;

 protected:
  db_base* basemodel;
  QSortFilterProxyModel* proxy;
  MainWindow* mainwin;

 public:
  XcaTreeView(QWidget* parent = nullptr);
  ~XcaTreeView() override;
  void contextMenuEvent(QContextMenuEvent* e) override;
  void setModel(QAbstractItemModel* model) override;
  void setMainwin(MainWindow* mw, QLineEdit* filter);
  QModelIndex getIndex(const QModelIndex& index);
  QModelIndex getProxyIndex(const QModelIndex& index);
  QModelIndexList getSelectedIndexes();
  void headerEvent(QContextMenuEvent* e, int col);
  QModelIndex currentIndex();
  void showContextMenu(QContextMenuEvent* e, const QModelIndex& index);
  virtual void fillContextMenu(QMenu* menu,
                               QMenu* subExport,
                               const QModelIndex& index,
                               QModelIndexList indexes) {
    (void)menu;
    (void)subExport;
    (void)index;
    (void)indexes;
  }
  void contextMenu(QContextMenuEvent* e,
                   QMenu* parent = nullptr,
                   int sect = -1);
  void keyPressEvent(QKeyEvent* event) override;
  virtual void showPki(pki_base*) {}
  virtual void exportItems(const QModelIndexList& indexes);
  virtual void load_default(load_base* load);
  virtual ExportDialog* exportDialog(const QModelIndexList& indexes);

 public slots:
  void changeView();
  void showHideSections();
  void sectionMoved(int idx, int oldI, int newI);
  void columnsResize();
  void editIdx();
  void setFilter(const QString& pattern);
  void deleteItems();
  void exportItems();
  void showItems();
  void newItem();
  void doubleClick(const QModelIndex& m);
  void pem2clipboard();
  void headerDetails();
  void columnRemove();
  void columnsChanged();
  void editComment();
  void showItem(pki_base*);
  void showItem(const QModelIndex& index);
  void showItem(const QString& name);
};

#endif  // XCA_TREE_VIEW_H
