#ifndef XCA_PKCS_11_LIB_H
#define XCA_PKCS_11_LIB_H

#include <QLibrary>

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QString>
#include "lib/exception.h"
#include "opensc_pkcs_11.h"

class pkcs11_lib final : public QLibrary {
  Q_OBJECT

 private:
  CK_FUNCTION_LIST* p11;
  QString file, load_error;
  bool is_enabled;

 public:
  static QString name2File(const QString& name, bool* enabled = nullptr);
  pkcs11_lib(const QString& file);
  ~pkcs11_lib() final;

  QList<unsigned long> getSlotList();
  [[nodiscard]] QString driverInfo() const;
  [[nodiscard]] QString filename() const { return file; }
  [[nodiscard]] CK_FUNCTION_LIST* ptr() const { return p11; }
  [[nodiscard]] bool isLoaded() const { return p11 != nullptr; }
  [[nodiscard]] enum Qt::CheckState checked() const {
    return is_enabled ? Qt::Checked : Qt::Unchecked;
  }
  [[nodiscard]] bool isLib(const QString& name) const {
    return name2File(name) == file;
  }
  [[nodiscard]] QString toData(int enabled) const {
    return QString("%1:%2").arg(enabled).arg(file);
  }
  [[nodiscard]] QString toData() const { return toData(is_enabled); }
  [[nodiscard]] QString pixmap() const {
    if (!is_enabled) return QString();
    return isLoaded() ? ":doneIco" : ":warnIco";
  }
};

class slotid {
 public:
  CK_ULONG id;
  pkcs11_lib* lib;
  slotid() = default;
  slotid(pkcs11_lib* l, CK_ULONG i) {
    lib = l;
    id = i;
  }
  void isValid() const {
    if (!lib) throw errorEx("InternalError: slotid is invalid");
  }
  [[nodiscard]] CK_FUNCTION_LIST* p11() const { return lib->ptr(); }
};

typedef QList<slotid> slotidList;

class pkcs11_lib_list final : public QAbstractListModel {
  QList<pkcs11_lib*> libs;
  QList<int> model_data;

 public:
  pkcs11_lib* add_lib(const QString& fname);
  void load(const QString& list);
  [[nodiscard]] slotidList getSlotList() const;
  [[nodiscard]] QString getPkcs11Provider() const;
  void remove_libs();
  [[nodiscard]] bool loaded() const;

  /* Helper for QAbstractListModel */
  [[nodiscard]] pkcs11_lib* libByModelIndex(const QModelIndex& index) const;

  /* Reimplementation from QAbstractListModel */
  [[nodiscard]] int rowCount(
      const QModelIndex& parent = QModelIndex()) const final;
  [[nodiscard]] QVariant data(const QModelIndex& index,
                              int role = Qt::DisplayRole) const final;
  bool setData(const QModelIndex& index, const QVariant& value, int role) final;

  [[nodiscard]] QMap<int, QVariant> itemData(
      const QModelIndex& index) const final;
  bool setItemData(const QModelIndex& index,
                   const QMap<int, QVariant>& roles) final;

  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const final;
  [[nodiscard]] Qt::DropActions supportedDropActions() const final;

  bool removeRows(int row,
                  int count,
                  const QModelIndex& p = QModelIndex()) final;
  bool insertRows(int row,
                  int count,
                  const QModelIndex& p = QModelIndex()) final;
};

[[noreturn]] void pk11error(const QString& fmt, int r);
[[noreturn]] void pk11error(const slotid& slot, const QString& func, int rv);
const char* pk11errorString(unsigned long rv);

#endif  // XCA_PKCS_11_LIB_H
