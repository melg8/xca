#ifndef DB_X_509_SUPER_H
#define DB_X_509_SUPER_H

#include "pki_x509super.h"

#include "db_key.h"

#include <QList>

class db_x509name : public db_base {
  Q_OBJECT

 protected:
  dbheaderList getHeaders() override;

 public:
  db_x509name(const char* classname);
};

class db_x509super : public db_x509name {
  Q_OBJECT

 protected:
  dbheaderList getHeaders() override;
  void loadContainer() override;

 public:
  db_x509super(const char* classname);
  pki_key* findKey(pki_x509super* ref);
  QList<pki_x509super*> findByPubKey(pki_key* refkey);
  void extractPubkey(QModelIndex index);
  void toTemplate(QModelIndex index);
};

#endif  // DB_X_509_SUPER_H
