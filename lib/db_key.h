#ifndef DB_KEY_H
#define DB_KEY_H

#include "db_base.h"

#include "pki_export.h"
#include "pki_key.h"
#include "x_file.h"

class QModelIndex;
class QContextMenuEvent;

class db_key final : public db_base {
  Q_OBJECT

 protected:
  dbheaderList getHeaders() final;

 public:
  db_key();
  QList<pki_key*> getUnusedKeys();
  QList<pki_key*> getAllKeys();
  pki_base* newPKI(enum pki_type type = none) final;
  void inToCont(pki_base* pki) final;
  void remFromCont(const QModelIndex& idx) final;
  pki_base* insert(pki_base* item) final;
  void setOwnPass(QModelIndex idx, enum pki_key::passType);
  void loadContainer() final;
  pki_key* newKey(const keyjob& task, const QString& name);
  int exportFlags(const QModelIndex& index) const final;
  void exportItem(const QModelIndex& index,
                  const pki_export* xport,
                  XFile& file) const final;

 signals:
  void delKey(pki_key* delkey);
  void newKey(pki_key* newkey);
  void keyDone(pki_key* nkey);
};

#endif  // DB_KEY_H
