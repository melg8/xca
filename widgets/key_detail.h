#ifndef KEY_DETAIL_H
#define KEY_DETAIL_H

#include "ui_key_detail.h"

class pki_key;
class pki_base;

class KeyDetail : public QDialog, public Ui::KeyDetail {
  Q_OBJECT

  QVariant keySqlId;

 public:
  KeyDetail(QWidget* w = nullptr);
  void setKey(pki_key* pKey);
  void setupFingerprints(pki_key* pKey);
  static void showKey(QWidget* parent, pki_key* pKey, bool ro = false);

 public slots:
  void itemChanged(pki_base* pki);
};

#endif  // KEY_DETAIL_H