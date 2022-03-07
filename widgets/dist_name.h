#ifndef DISTNAME_H
#define DISTNAME_H

#include <QGridLayout>
#include <QWidget>

class x509name;
class QLabel;
class QComboBox;
class QLineEdit;
class myGridlayout;

class DistName : public QWidget {
  Q_OBJECT

 public:
  DistName(QWidget* parent);
  void setX509name(const x509name& n);

 protected:
  QGridLayout* DistNameLayout;
  QLineEdit* rfc2253;
  QLineEdit* namehash;
};

#endif  // DISTNAME_H
