#ifndef FOCUS_COMBO_H
#define FOCUS_COMBO_H

#include <QComboBox>

class focusCombo final : public QComboBox {
 public:
  focusCombo(QWidget* parent) : QComboBox(parent) {}
  void hidePopup() final {
    QComboBox::hidePopup();
    emit highlighted(currentIndex());
  }
  QVariant currentItemData() { return QComboBox::itemData(currentIndex()); }
  void addItemsData(const QStringList& textdata, const QString& selected) {
    int c = 0;
    Q_ASSERT(textdata.size() % 2 == 0);
    for (int i = 0; i < textdata.size(); i += 2) {
      addItem(textdata[i], textdata[i + 1]);
      if (textdata[i + 1] == selected) c = i / 2;
    }
    setCurrentIndex(c);
  }
};

#endif  // FOCUS_COMBO_H
