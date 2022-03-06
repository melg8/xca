#ifndef __OID_RESOLVER_H
#define __OID_RESOLVER_H

#include <QWidget>
#include "ui_OidResolver.h"

class OidResolver : public QWidget, public Ui::OidResolver {
  Q_OBJECT

 public:
  OidResolver(QWidget* w);

 public slots:
  void searchOid(QString s);
};

#endif
