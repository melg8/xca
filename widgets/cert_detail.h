#ifndef CERT_DETAIL_H
#define CERT_DETAIL_H

#include <QDialog>
#include "ui_cert_detail.h"

#include "lib/pki_x_509.h"
#include "lib/pki_x_509_req.h"

class pki_x509;

class CertDetail final : public QDialog, public Ui::CertDetail {
  Q_OBJECT

  bool showConf;
  QVariant keySqlId, issuerSqlId, thisSqlId;
  QString conf, exts;
  QLabel* labelFromAsn1String(ASN1_STRING* s);
  pki_key *myPubKey, *tmpPubKey;
  void setCert(pki_x509* cert);
  void setReq(pki_x509req* req);

 public:
  CertDetail(QWidget* w = nullptr);
  ~CertDetail() final;
  void setX509super(pki_x509super* x);
  static void showCert(QWidget* parent, pki_x509super* x);

 private slots:
  void on_showExt_clicked();
  void itemChanged(pki_base* pki);
  void showPubKey();
  void showIssuer();
};

#endif  // CERT_DETAIL_H