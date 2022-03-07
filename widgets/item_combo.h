#ifndef ITEM_COMBO_H
#define ITEM_COMBO_H

#include <QComboBox>
#include <QList>

#include "lib/pki_base.h"
#include "lib/pki_temp.h"
#include "lib/pki_x_509.h"
#include "lib/pki_x_509_req.h"

template <class T>
class itemCombo : public QComboBox {
 public:
  itemCombo(QWidget* parent) : QComboBox(parent) {}
  void insertPkiItems(QList<T*> items) {
    clear();
    foreach (T* p, items) { addItem(p->comboText(), QVariant::fromValue(p)); }
  }
  T* currentPkiItem() { return itemData(currentIndex()).template value<T*>(); }
  void setNullItem(QString text) {
    if (itemData(0).template value<T*>() == nullptr) removeItem(0);
    insertItem(0, text, QVariant());
  }
  int setCurrentPkiItem(T* p) {
    int idx = findData(QVariant::fromValue(p));
    setCurrentIndex(idx);
    return idx;
  }
};

typedef class itemCombo<pki_temp> itemComboTemp;
typedef class itemCombo<pki_x509req> itemComboReq;
typedef class itemCombo<pki_x509> itemComboCert;
typedef class itemCombo<pki_key> itemComboKey;

#endif  // ITEM_COMBO_H
