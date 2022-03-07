#ifndef DB_BASE_H
#define DB_BASE_H

#include <QAbstractItemModel>

#include <typeinfo>

#include "base.h"
#include "pki_base.h"  // <------------|
// TODO(melg): fix headers dependency  |-- if swapped - doesn't compile.
#include "header_list.h"  // <----------|

#include <QHash>
#include <QStringList>

#define X_XCA_DRAG_DATA "application/x-xca-drag-data"

class QContextMenuEvent;
class XcaTreeView;
class NewX509;

class db_base : public QAbstractItemModel {
  Q_OBJECT

 protected:
  int secondsTimer, minutesTimer, hoursTimer;
  void _writePKI(pki_base* pki, bool overwrite);
  QList<enum pki_type> pkitype;
  QList<enum pki_type> pkitype_depends;
  QString class_name;
  /* Sql table containing the 'hash' of this items */
  QString sqlHashTable;
  dbheaderList allHeaders;
  virtual dbheaderList getHeaders();
  int colResizing;
  QString sqlItemSelector();
  [[nodiscard]] bool isValidCol(int col) const;
  void timerEvent(QTimerEvent* event) override;
  bool treeview;
  pki_base* rootItem;
  pki_base* treeItem;

 public:
  void restart_timer();
  void updateItem(pki_base* pki, const QString& name, const QString& comment);

  virtual pki_base* newPKI(enum pki_type type = none);
  db_base(const char* classname);
  virtual void updateHeaders();
  ~db_base() override;
  virtual pki_base* insertPKI(pki_base* pki);
  pki_base* getByName(QString desc);
  pki_base* getByReference(pki_base* refpki);
  virtual void loadContainer();
  void reloadContainer(const QList<enum pki_type>& typelist);
  virtual pki_base* insert(pki_base* item);
  virtual void inToCont(pki_base* pki);
  virtual void remFromCont(const QModelIndex& idx);
  void changeView();
  [[nodiscard]] int exportFlags(const QModelIndexList& indexes) const;
  [[nodiscard]] virtual int exportFlags(const QModelIndex&) const { return 0; }
  virtual void exportItem(const QModelIndex&, const pki_export*, XFile&) const {
  }
  virtual void exportItems(const QModelIndexList& indexes,
                           const pki_export* xport,
                           XFile& file) const;

  void dump(const QString& dirname) const;
  [[nodiscard]] QModelIndex index(int row,
                                  int column,
                                  const QModelIndex& parent) const override;
  QModelIndex index(pki_base* pki) const;
  [[nodiscard]] QModelIndex parent(const QModelIndex& index) const override;
  [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
  [[nodiscard]] int allItemsCount() const { return rootItem->childCount(); }
  [[nodiscard]] int columnCount(const QModelIndex& parent) const override;
  [[nodiscard]] QVariant data(const QModelIndex& index,
                              int role) const override;
  [[nodiscard]] QVariant headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const override;
  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
  bool setData(const QModelIndex& index,
               const QVariant& value,
               int role) override;
  void deleteSelectedItems(QModelIndexList indexes);
  static pki_base* fromIndex(const QModelIndex& index) {
    if (!index.isValid()) return nullptr;
    return static_cast<pki_base*>(index.internalPointer());
  }
  template <class T>
  static T* fromIndex(const QModelIndex& index) {
    return dynamic_cast<T*>(fromIndex(index));
  }
  void insertChild(pki_base* child, pki_base* parent = nullptr);
  int rownumber(const pki_base* child) const;
  static void createSuccess(const pki_base* pki);
  [[nodiscard]] bool columnHidden(int col) const;
  virtual void saveHeaderState();
  void initHeaderView(QHeaderView* hv);
  void setVisualIndex(int i, int visualIndex);
  bool fixedHeaderSize(int sect);
  bool treeViewMode() { return treeview; }
  void colResizeStart() { colResizing++; }
  void colResizeEnd() { colResizing--; }
  dbheaderList getAllHeaders() { return allHeaders; }
  [[nodiscard]] QString pem2QString(QModelIndexList indexes) const;

  void deletePKI(QModelIndex idx);
  [[nodiscard]] QMimeData* mimeData(
      const QModelIndexList& indexes) const override;
  void emitDataChanged(pki_base* pki);
  [[nodiscard]] bool containsType(enum pki_type t) const;
  void writeVcalendar(XFile& file, QStringList vcal) const;

 public slots:
  virtual void newItem() {}
  void columnResetDefaults();
  void sectionResized(int i, int, int newSize);
  void sortIndicatorChanged(int, Qt::SortOrder);

 signals:
  void resetHeader() const;
  void updateHeader() const;
  void columnsContentChanged() const;
  void pkiChanged(pki_base* pki) const;
};

#endif  // DB_BASE_H
