#ifndef CRL_DETAIL_H
#define CRL_DETAIL_H

#include "ui_crl_detail.h"

class pki_crl;
class pki_base;

class CrlDetail : public QDialog, public Ui::CrlDetail {
  Q_OBJECT

 private:
  QVariant issuerSqlId, crlSqlId;

 public:
  CrlDetail(QWidget* w = nullptr);
  void setCrl(pki_crl* crl);
  static void showCrl(QWidget* parent, pki_crl* crl);
 public slots:
  void itemChanged(pki_base* pki);
  void showIssuer();
};

#endif  // CRL_DETAIL_H