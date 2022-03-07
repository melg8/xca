#ifndef XCA_PKCS_11_H
#define XCA_PKCS_11_H

#include <QList>
#include <QString>
#include <QStringList>
#include "opensc_pkcs_11.h"
#include "pkcs_11_lib.h"

#include "func.h"
#include "pk_11_attribute.h"

void waitcursor(int start, int line);
#define WAITCURSOR_START waitcursor(1, __LINE__) s
#define WAITCURSOR_END waitcursor(0, __LINE__)

extern char segv_data[1024];
#define CALL_P11_C(l, func, ...)                                              \
  do {                                                                        \
    snprintf(segv_data, sizeof segv_data,                                     \
             "Crashed in %s in %s from %s:%d\n"                               \
             "This looks like a bug in the PKC#11 library and not in XCA\n",  \
             #func, CCHAR((l)->filename()), __func__, __LINE__);              \
    if (IS_GUI_APP) QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); \
    rv = l->ptr()->func(__VA_ARGS__);                                         \
    segv_data[0] = 0;                                                         \
    if (IS_GUI_APP) QApplication::restoreOverrideCursor();                    \
    ign_openssl_error();                                                      \
  } while (0)

class tkInfo {
 private:
  CK_TOKEN_INFO token_info;

 public:
  tkInfo() { memset(&token_info, 0, sizeof token_info); }
  tkInfo(const CK_TOKEN_INFO* ti) { set(ti); }
  tkInfo(const tkInfo& tk) { set(&tk.token_info); }
  void set(const CK_TOKEN_INFO* ti) {
    memcpy(&token_info, ti, sizeof(token_info));
    // sanitize strings
    for (int i = 0; i < 32; i++) {
      if (token_info.label[i] == 0) token_info.label[i] = ' ';
      if (token_info.manufacturerID[i] == 0) token_info.manufacturerID[i] = ' ';
    }
    for (int i = 0; i < 16; i++) {
      if (token_info.model[i] == 0) token_info.model[i] = ' ';
      if (token_info.serialNumber[i] == 0) token_info.serialNumber[i] = ' ';
    }
  }
  [[nodiscard]] QString label() const {
    return UTF8QSTRING(token_info.label, 32);
  }
  [[nodiscard]] QString manufacturerID() const {
    return UTF8QSTRING(token_info.manufacturerID, 32);
  }
  [[nodiscard]] QString model() const {
    return UTF8QSTRING(token_info.model, 16);
  }
  [[nodiscard]] QString serial() const {
    return ASCIIQSTRING(token_info.serialNumber, 16);
  }
  [[nodiscard]] bool protAuthPath() const {
    return !!(token_info.flags & CKF_PROTECTED_AUTHENTICATION_PATH);
  }
  [[nodiscard]] bool tokenInitialized() const {
    return !!(token_info.flags & CKF_TOKEN_INITIALIZED);
  }
  [[nodiscard]] QString pinInfo() const {
    return QObject::tr("Required PIN size: %1 - %2")
        .arg(token_info.ulMinPinLen)
        .arg(token_info.ulMaxPinLen);
  }
};

class pkcs11 {
  friend class pk11_attribute;
  friend class pk11_attr_ulong;
  friend class pk11_attr_data;

 private:
  slotid p11slot;
  CK_SESSION_HANDLE session;
  CK_OBJECT_HANDLE p11obj;

 public:
  static pkcs11_lib_list libraries;
  pkcs11();
  ~pkcs11();

  CK_RV tokenInfo(const slotid& slot, tkInfo* tkinfo);
  tkInfo tokenInfo(const slotid& slot);
  tkInfo tokenInfo() { return tokenInfo(p11slot); }
  [[nodiscard]] QString driverInfo(const slotid& slot) const {
    return slot.lib->driverInfo();
  }
  static slotidList getSlotList() { return libraries.getSlotList(); }

  bool selectToken(slotid* slot, QWidget* w);
  void changePin(const slotid& slot, bool so);
  void initPin(const slotid& slot);
  void initToken(const slotid& slot,
                 const unsigned char* pin,
                 int pinlen,
                 QString label);
  QList<CK_MECHANISM_TYPE> mechanismList(const slotid& slot);
  void mechanismInfo(const slotid& slot,
                     CK_MECHANISM_TYPE m,
                     CK_MECHANISM_INFO* info);
  void startSession(const slotid& slot, bool rw = false);

  /* Session based functions */
  void loadAttribute(pk11_attribute& attribute, CK_OBJECT_HANDLE object);
  void storeAttribute(pk11_attribute& attribute, CK_OBJECT_HANDLE object);
  QList<CK_OBJECT_HANDLE> objectList(pk11_attlist& atts);
  QString tokenLogin(QString name, bool so, bool force = false);
  void getRandom();
  void logout();
  bool needsLogin(bool so);
  void login(const unsigned char* pin, unsigned long pinlen, bool so);

  void setPin(const unsigned char* oldPin,
              const unsigned long oldPinLen,
              const unsigned char* pin,
              unsigned long pinLen);
  CK_OBJECT_HANDLE createObject(pk11_attlist& attrs);
  pk11_attr_data findUniqueID(unsigned long oclass);
  pk11_attr_data generateKey(QString name,
                             unsigned long ec_rsa_mech,
                             unsigned long bits,
                             int nid);
  int deleteObjects(QList<CK_OBJECT_HANDLE> objects);
  EVP_PKEY* getPrivateKey(EVP_PKEY* pub, CK_OBJECT_HANDLE obj);
  int encrypt(int flen,
              const unsigned char* from,
              unsigned char* to,
              int tolen,
              unsigned long m);
  int decrypt(int flen,
              const unsigned char* from,
              unsigned char* to,
              int tolen,
              unsigned long m);
};

#endif  // XCA_PKCS_11_H
