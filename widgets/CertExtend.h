#ifndef __CERTEXTEND_H
#define __CERTEXTEND_H

#include "lib/pki_x509.h"
#include "ui_CertExtend.h"

class pki_key;

class CertExtend final : public QDialog, public Ui::CertExtend {
  Q_OBJECT

  pki_x509* signer;

 public:
  CertExtend(QWidget* parent, pki_x509* s);

 public slots:
  void on_applyTime_clicked();
  void accept() final;
};

#endif
