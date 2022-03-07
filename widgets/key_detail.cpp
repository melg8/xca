#include "key_detail.h"

#include "lib/main.h"
#include "lib/pki_evp.h"
#include "lib/pki_scard.h"

#include "click_label.h"
#include "dist_name.h"
#include "help.h"
#include "main_window.h"
#include "oid_resolver.h"
#include "xca_application.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

KeyDetail::KeyDetail(QWidget* w) : QDialog(w ? w : mainwin), keySqlId() {
  setupUi(this);
  setWindowTitle(XCA_TITLE);
  image->setPixmap(QPixmap(":keyImg"));
  mainwin->helpdlg->register_ctxhelp_button(this, "keydetail");

  keyModulus->setFont(XcaApplication::tableFont);
  tabWidget->setCurrentIndex(0);

  Database.connectToDbChangeEvt(this, SLOT(itemChanged(pki_base*)));
}

#ifndef OPENSSL_NO_EC
static QString CurveComment(int nid) {
  foreach (builtin_curve curve, builtinCurves) {
    if (curve.nid == nid) return curve.comment;
  }
  return {};
}
#endif

void KeyDetail::setupFingerprints(pki_key* pKey) {
  int pos = 0;
  auto* widget = new QWidget(fingerprint);
  auto* v = new QVBoxLayout(fingerprint);
  auto* grid = new QGridLayout(widget);
  v->addStretch();
  v->addWidget(widget);
  v->addStretch();

  QStringList sl;
  sl << "ssh MD5"
     << "ssh SHA256 B64"
     << "x509 SHA1"
     << "DER SHA256";

  foreach (QString type, sl) {
    qDebug() << type << pKey->fingerprint(type);

    auto* left = new QLabel(widget);
    auto* right = new CopyLabel(widget);

    left->setTextFormat(Qt::PlainText);
    left->setText(type);
    right->setText(pKey->fingerprint(type));

    grid->addWidget(left, pos, 0);
    grid->addWidget(right, pos, 1);
    pos++;
  }
}

void KeyDetail::setKey(pki_key* pKey) {
  keySqlId = pKey->getSqlItemId();
  keyDesc->setText(pKey->getIntName());
  keyLength->setText(pKey->length());

  keyPrivEx->disableToolTip();
  if (!pKey->isToken()) tabWidget->removeTab(1);
  tlHeader->setText(tr("Details of the %1 key").arg(pKey->getTypeString()));
  comment->setPlainText(pKey->getComment());

  setupFingerprints(pKey);

  if (pKey->isPubKey()) {
    keyPrivEx->setText(tr("Not available"));
    keyPrivEx->setRed();
  } else if (pKey->isToken()) {
    image->setPixmap(QPixmap(":scardImg"));
    auto* card = static_cast<pki_scard*>(pKey);
    cardLabel->setText(card->getCardLabel());
    cardModel->setText(card->getModel());
    cardManufacturer->setText(card->getManufacturer());
    cardSerial->setText(card->getSerial());
    slotLabel->setText(card->getLabel());
    cardId->setText(card->getId());
    keyPrivEx->setText(tr("Security token"));
  } else {
    keyPrivEx->setText(tr("Available"));
    keyPrivEx->setGreen();
  }
  switch (pKey->getKeyType()) {
    case EVP_PKEY_RSA:
      keyPubEx->setText(pKey->pubEx());
      keyModulus->setText(pKey->modulus());
      break;
    case EVP_PKEY_DSA:
      tlPubEx->setText(tr("Sub prime"));
      tlModulus->setTitle(tr("Public key"));
      tlPrivEx->setText(tr("Private key"));
      keyPubEx->setText(pKey->subprime());
      keyModulus->setText(pKey->pubkey());
      break;
#ifndef OPENSSL_NO_EC
    case EVP_PKEY_EC:
      int nid;
      nid = pKey->ecParamNid();
      tlModulus->setTitle(tr("Public key"));
      tlPrivEx->setText(tr("Private key"));
      tlPubEx->setText(tr("Curve name"));
      keyPubEx->setText(OBJ_nid2sn(nid));
      connect(keyPubEx, SIGNAL(doubleClicked(QString)),
              MainWindow::getResolver(), SLOT(searchOid(QString)));
      keyPubEx->setToolTip(CurveComment(nid));
      keyModulus->setText(pKey->ecPubKey());
      break;
#ifdef EVP_PKEY_ED25519
    case EVP_PKEY_ED25519:
      tlModulus->setTitle(tr("Public key"));
      tlPrivEx->setText(tr("Private key"));
      tlPubEx->setText(tr("Curve name"));
      keyPubEx->setText("ed25519");
      keyModulus->setText(pKey->ed25519PubKey().toHex());
      break;
#endif
#endif
    default:
      tlHeader->setText(tr("Unknown key"));
  }
}

void KeyDetail::itemChanged(pki_base* pki) {
  if (pki->getSqlItemId() == keySqlId) keyDesc->setText(pki->getIntName());
}

void KeyDetail::showKey(QWidget* parent, pki_key* pKey, bool ro) {
  if (!pKey) return;
  auto* dlg = new KeyDetail(parent);
  if (!dlg) return;
  dlg->setKey(pKey);
  dlg->keyDesc->setReadOnly(ro);
  dlg->comment->setReadOnly(ro);
  if (dlg->exec()) {
    db_base* db = Database.modelForPki(pKey);
    if (!db) {
      pKey->setIntName(dlg->keyDesc->text());
      pKey->setComment(dlg->comment->toPlainText());
    } else {
      db->updateItem(pKey, dlg->keyDesc->text(), dlg->comment->toPlainText());
    }
  }
  delete dlg;
}
