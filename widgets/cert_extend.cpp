#include "cert_extend.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include "lib/base.h"
#include "lib/func.h"
#include "widgets/validity.h"
#include "widgets/xca_warning.h"

CertExtend::CertExtend(QWidget* parent, pki_x509* s) : QDialog(parent) {
  setupUi(this);
  setWindowTitle(XCA_TITLE);
  image->setPixmap(QPixmap(":certImg"));
  validNumber->setText("1");
  validRange->setCurrentIndex(2);
  on_applyTime_clicked();
  signer = s;
  notAfter->setEndDate(true);
}

void CertExtend::on_applyTime_clicked() {
  notAfter->setDiff(notBefore, validNumber->text().toInt(),
                    validRange->currentIndex());
}

void CertExtend::accept() {
  if (signer && notBefore->getDate() < signer->getNotBefore()) {
    QString text =
        tr("The certificate will be earlier valid than the signer. This is "
           "probably not what you want.");
    xcaWarningBox msg(this, text);
    msg.addButton(QMessageBox::Ok, tr("Edit dates"));
    msg.addButton(QMessageBox::Close, tr("Abort rollout"));
    msg.addButton(QMessageBox::Apply, tr("Continue rollout"));
    msg.addButton(QMessageBox::Yes, tr("Adjust date and continue"));
    switch (msg.exec()) {
      case QMessageBox::Ok:
      case QMessageBox::Cancel:
        return;
      case QMessageBox::Close:
        reject();
        return;
      case QMessageBox::Apply:
        break;
      case QMessageBox::Yes:
        notBefore->setDate(signer->getNotBefore());
    }
  }
  if (signer && notAfter->getDate() > signer->getNotAfter() &&
      !noWellDefinedExpDate->isChecked()) {
    QString text =
        tr("The certificate will be longer valid than the signer. This is "
           "probably not what you want.");
    xcaWarningBox msg(this, text);
    msg.addButton(QMessageBox::Ok, tr("Edit dates"));
    msg.addButton(QMessageBox::Close, tr("Abort rollout"));
    msg.addButton(QMessageBox::Apply, tr("Continue rollout"));
    msg.addButton(QMessageBox::Yes, tr("Adjust date and continue"));
    switch (msg.exec()) {
      case QMessageBox::Ok:
      case QMessageBox::Cancel:
        return;
      case QMessageBox::Close:
        reject();
        return;
      case QMessageBox::Apply:
        break;
      case QMessageBox::Yes:
        notAfter->setDate(signer->getNotAfter());
    }
  }
  QDialog::accept();
}