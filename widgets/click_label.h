#ifndef CLICK_LABEL_H
#define CLICK_LABEL_H

#include <QLabel>

class QMouseEvent;

class DoubleClickLabel : public QLabel {
  Q_OBJECT

  QString clicktext;

 public:
  DoubleClickLabel(QWidget* parent) : QLabel(parent) {}
  void setClickText(QString s);

 protected:
  void mouseDoubleClickEvent(QMouseEvent* e) override;

 signals:
  void doubleClicked(QString text);
};

class ClickLabel : public DoubleClickLabel {
  Q_OBJECT

 public:
  ClickLabel(QWidget* parent);
  void setRed();
  void setGreen();
  void disableToolTip();

 protected:
  void setColor(const QColor& col);
};

class CopyLabel : public DoubleClickLabel {
  Q_OBJECT

 public:
  CopyLabel(QWidget* parent);
};

#endif  // CLICK_LABEL_H