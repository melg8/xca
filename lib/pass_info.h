#ifndef PASS_INFO_H
#define PASS_INFO_H

#include <QObject>

#include <QApplication>
#include <QString>

#include "lib/exception.h"

class QWidget;

class pass_info : public QObject {
  Q_OBJECT

 private:
  QString title;
  QString description;
  QWidget* widget;
  QString type;
  QString pixmap;
  enum open_result result;

 public:
  pass_info(const QString& t, const QString& d, QWidget* w = nullptr);
  [[nodiscard]] QString getTitle() const { return title; }
  [[nodiscard]] QString getDescription() const { return description; }
  QWidget* getWidget() {
    if (!widget) {
      widget = qApp->activeWindow();
    }
    return widget;
  }
  [[nodiscard]] QString getType() const { return type; }
  [[nodiscard]] QString getImage() const { return pixmap; }
  [[nodiscard]] enum open_result getResult() const { return result; }
  void setTitle(QString t) { title = t; }
  void setDescription(QString d) { description = d; }
  void setWidget(QWidget* w) { widget = w; }
  void setResult(enum open_result r) { result = r; }
  void setPin();
};

#endif  // PASS_INFO_H
