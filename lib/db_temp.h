#ifndef DB_TEMP_H
#define DB_TEMP_H

#include "db_x_509_super.h"

class pki_temp;

class db_temp final : public db_x509name {
  Q_OBJECT
 protected:
  QList<pki_temp*> predefs;

 public:
  db_temp();
  ~db_temp() final;
  pki_base* newPKI(enum pki_type type = none) final;
  void fillContextMenu(QMenu* menu, const QModelIndex& index);
  [[nodiscard]] QList<pki_temp*> getPredefs() const;
  bool alterTemp(pki_temp* temp);
  void exportItem(const QModelIndex& index,
                  const pki_export*,
                  XFile& file) const final;
};

#endif  // DB_TEMP_H
