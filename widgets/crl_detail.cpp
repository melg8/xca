#include "crl_detail.h"

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include "cert_detail.h"
#include "click_label.h"
#include "dist_name.h"
#include "help.h"
#include "lib/pki_crl.h"
#include "main_window.h"
#include "oid_resolver.h"
#include "open_db.h"
#include "revocation_list.h"

CrlDetail::CrlDetail(QWidget* w) : QDialog(w ? w : mainwin) {
  setupUi(this);
  setWindowTitle(XCA_TITLE);
  mainwin->helpdlg->register_ctxhelp_button(this, "crldetail");

  image->setPixmap(QPixmap(":revImg"));
  Database.connectToDbChangeEvt(this, SLOT(itemChanged(pki_base*)));
}

static QString AsHex(long value) noexcept {
  return QString("%1").arg(value, 2, 16, QLatin1Char('0'));
}

void CrlDetail::setCrl(pki_crl* crl) {
  pki_x509* iss;
  x509v3ext e1;
  x509v3ext e2;

  iss = crl->getIssuer();
  crlSqlId = crl->getSqlItemId();

  signCheck->disableToolTip();
  signCheck->setClickText(crl->getSigAlg());
  if (iss != nullptr) {
    issuerIntName->setText(iss->getIntName());
    issuerIntName->setClickText(iss->getSqlItemId().toString());
    issuerIntName->setGreen();
    if (crl->verify(iss)) {
      signCheck->setText(crl->getSigAlg());
      signCheck->setGreen();
    } else {
      signCheck->setText(tr("Failed"));
      signCheck->setRed();
    }
    issuerSqlId = iss->getSqlItemId();
  } else {
    issuerIntName->setText(tr("Unknown signer"));
    issuerIntName->setDisabled(true);
    issuerIntName->disableToolTip();
    signCheck->setText(tr("Verification not possible"));
    signCheck->setDisabled(true);
  }

  connect(signCheck, SIGNAL(doubleClicked(QString)), MainWindow::getResolver(),
          SLOT(searchOid(QString)));
  connect(issuerIntName, SIGNAL(doubleClicked(QString)), this,
          SLOT(showIssuer()));

  descr->setText(crl->getIntName());
  lUpdate->setText(crl->getLastUpdate().toPretty());
  lUpdate->setToolTip(crl->getLastUpdate().toPrettyGMT());
  nUpdate->setText(crl->getNextUpdate().toPretty());
  nUpdate->setToolTip(crl->getNextUpdate().toPrettyGMT());
  version->setText(AsHex(crl->getVersion() + 1));

  issuer->setX509name(crl->getSubject());

  RevocationList::setupRevocationView(certList, crl->getRevList(), iss);

  v3extensions->document()->setHtml(crl->printV3ext());

  comment->setPlainText(crl->getComment());
}

void CrlDetail::itemChanged(pki_base* pki) {
  QVariant pkiSqlId = pki->getSqlItemId();

  if (pkiSqlId == issuerSqlId) {
    issuerIntName->setText(pki->getIntName());
  }
  if (pkiSqlId == crlSqlId) {
    descr->setText(pki->getIntName());
  }
}

void CrlDetail::showIssuer() {
  CertDetail::showCert(this, Store.lookupPki<pki_x509>(issuerSqlId));
}

void CrlDetail::showCrl(QWidget* parent, pki_crl* crl) {
  auto* dlg = new CrlDetail(parent);
  if (!dlg) {
    return;
  }

  dlg->setCrl(crl);
  if (dlg->exec()) {
    db_base* db = Database.modelForPki(crl);
    if (!db) {
      crl->setIntName(dlg->descr->text());
      crl->setComment(dlg->comment->toPlainText());
    } else {
      db->updateItem(crl, dlg->descr->text(), dlg->comment->toPlainText());
    }
  }
  delete dlg;
}
