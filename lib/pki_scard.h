#ifndef PKI_SCARD_H
#define PKI_SCARD_H

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <QString>

#include "pkcs_11.h"
#include "pki_key.h"

#define VIEW_tokens_card_manufacturer 10
#define VIEW_tokens_card_serial 11
#define VIEW_tokens_card_model 12
#define VIEW_tokens_card_label 13
#define VIEW_tokens_slot_label 14
#define VIEW_tokens_object_id 15

class pki_scard final : public pki_key {
  Q_OBJECT
 protected:
  QString card_serial;
  QString card_manufacturer;
  QString card_model;
  QString card_label;
  QString slot_label;
  QString object_id;
  QList<CK_MECHANISM_TYPE> mech_list;
  void init();

 public:
  pki_scard(const QString name);
  ~pki_scard() final;
  void load_token(pkcs11& p11, CK_OBJECT_HANDLE object);
  bool prepare_card(slotid* slot) const;
  bool find_key_on_card(slotid* slot) const;
  QString getTypeString() const final;
  QString getManufacturer() const { return card_manufacturer; }
  QString getSerial() const { return card_serial; }
  QString getModel() const { return card_model; }
  QString getLabel() const { return slot_label; }
  void updateLabel(QString label);
  QString getId() const { return object_id; }
  pk11_attr_data getIdAttr() const;
  QString getCardLabel() const { return card_label; }
  EVP_PKEY* decryptKey() const final;
  QString scardLogin(pkcs11& p11, bool so, bool force = false) const;
  void changePin();
  void initPin();
  void changeSoPin();
  int verify();
  bool isToken() final;
  QVariant getIcon(const dbheader* hd) const final;
  QList<CK_MECHANISM_TYPE> getMech_list() { return mech_list; }
  pk11_attlist objectAttributes(bool priv) const;
  pk11_attlist objectAttributesNoId(EVP_PKEY* pk, bool priv) const;
  void setMech_list(QList<CK_MECHANISM_TYPE> ml) { mech_list = ml; }
  QList<int> possibleHashNids() final;
  EVP_PKEY* load_pubkey(pkcs11& p11, CK_OBJECT_HANDLE object) const;
  void generate(const keyjob& task) final;
  void deleteFromToken() final;
  void deleteFromToken(const slotid& slot) final;
  void store_token(const slotid& slot, EVP_PKEY* pkey);
  int renameOnToken(const slotid& slot, const QString& name) final;
  QString getMsg(msg_type msg) const final;
  bool visible() const final;
  QSqlError insertSqlData() final;
  QSqlError deleteSqlData() final;
  void restoreSql(const QSqlRecord& rec) final;
};

#endif  // PKI_SCARD_H
