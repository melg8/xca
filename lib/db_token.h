#ifndef __DB_TOKEN_H
#define __DB_TOKEN_H

#include "db_base.h"

#include "pkcs11_lib.h"

class pki_scard;
class db_token final : public db_base {
  Q_OBJECT
 private:
  slotid slot;

 public:
  db_token();
  bool setData(const QModelIndex& index, const QVariant& value, int role) final;
  void setSlot(const slotid& s) { slot = s; }
  void saveHeaderState() final;
  void rename_token_in_database(pki_scard* token);
};

#endif
