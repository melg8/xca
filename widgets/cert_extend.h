#ifndef CERT_EXTEND_H
#define CERT_EXTEND_H

#include "lib/pki_x_509.h"
#include "ui_cert_extend.h"

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

#endif  // CERT_EXTEND_H