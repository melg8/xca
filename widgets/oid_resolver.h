#ifndef OID_RESOLVER_H
#define OID_RESOLVER_H

#include <QWidget>
#include "ui_oid_resolver.h"

class OidResolver : public QWidget, public Ui::OidResolver {
  Q_OBJECT

 public:
  OidResolver(QWidget* w);

 public slots:
  void searchOid(QString s);
};

#endif  // OID_RESOLVER_H
