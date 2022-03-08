#ifndef DB_X_509_REQ_H
#define DB_X_509_REQ_H

#include "db_x_509_super.h"

class pki_temp;
class pki_x509req;

class db_x509req final : public db_x509super {
  Q_OBJECT

 protected:
  dbheaderList getHeaders() final;

 public:
  db_x509req();
  pki_base* insert(pki_base* item) final;
  pki_base* newPKI(enum pki_type type = none) final;
  void fillContextMenu(QMenu* menu, const QModelIndex& index);
  static QList<pki_x509req*> getAllRequests();
  static void resetX509count();
  void setSigned(QModelIndex index, bool signe);
  void exportItem(const QModelIndex& index,
                  const pki_export* xport,
                  XFile& file) const final;

 public slots:
  void newItem(pki_temp* temp, pki_x509req* orig = nullptr);
  void newItem() final;
};

#endif  // DB_X_509_REQ_H
