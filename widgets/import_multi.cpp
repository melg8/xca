#include "import_multi.h"

#include "cert_detail.h"
#include "crl_detail.h"
#include "key_detail.h"
#include "lib/pki_base.h"
#include "lib/pki_crl.h"
#include "lib/pki_evp.h"
#include "lib/pki_multi.h"
#include "lib/pki_pkcs_12.h"
#include "lib/pki_pkcs_7.h"
#include "lib/pki_scard.h"
#include "lib/pki_temp.h"
#include "main_window.h"
#include "xca_warning.h"

#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QUrl>
#include <typeinfo>

ImportMulti::ImportMulti(QWidget* parent) : QDialog(parent ? parent : mainwin) {
  setupUi(this);
  setWindowTitle(XCA_TITLE);
  image->setPixmap(QPixmap(":certImg"));
  listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  mcont = new db_token();
  listView->setModel(mcont);
  listView->setIconSize(QPixmap(":key").size());
  listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this,
          SLOT(on_butDetails_clicked()));
  deleteToken->hide();
  renameToken->hide();
  slotInfo->hide();
  setAcceptDrops(true);
  setWindowModality(Qt::WindowModal);
}

void ImportMulti::tokenInfo(const slotid& s) {
  slot = s;
  mcont->setSlot(slot);
  deleteToken->show();
  renameToken->show();
  slotInfo->show();
  listView->setEditTriggers(QAbstractItemView::EditKeyPressed);

  pkcs11 p11;

  QString info = p11.driverInfo(slot);
  tkInfo ti = p11.tokenInfo(slot);
  info += tr("\nName: %1\nModel: %2\nSerial: %3")
              .arg(ti.label())
              .arg(ti.model())
              .arg(ti.serial());

  slotInfo->setText(info);
  image->setPixmap(QPixmap(":scardImg"));
  heading->setText(tr("Manage security token"));
  setAcceptDrops(false);
}

void ImportMulti::addItem(pki_base* pki) {
  if (!pki) return;

  if (pki->pkiSource == unknown) pki->pkiSource = imported;
  auto* pm = dynamic_cast<pki_multi*>(pki);
  if (pm) {
    QList<pki_base*> items = pm->pull();
    foreach (pki_base* inner, items)
      addItem(inner);
    delete pm;
    return;
  }

  auto* cert = dynamic_cast<pki_x509*>(pki);
  auto* crl = dynamic_cast<pki_crl*>(pki);
  auto* cert_or_req = dynamic_cast<pki_x509super*>(pki);

  if (cert) cert->setSigner(cert->findIssuer());
  if (cert_or_req) cert_or_req->lookupKey();
  if (crl) crl->lookupIssuer();

  if (!dynamic_cast<pki_key*>(pki) && !dynamic_cast<pki_x509name*>(pki)) {
    XCA_WARN(tr("The type of the item '%1' is not recognized")
                 .arg(pki->getClassName()));
    delete pki;
    return;
  }
  mcont->inToCont(pki);
  mcont->rename_token_in_database(dynamic_cast<pki_scard*>(pki));
}

bool ImportMulti::openDB() const {
  if (!Database.isOpen()) {
    if (mainwin->init_database(QString()) == 2) return false;
    if (!Database.isOpen()) mainwin->load_database();
  }
  return Database.isOpen();
}

void ImportMulti::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void ImportMulti::dropEvent(QDropEvent* event) {
  QList<QUrl> urls = event->mimeData()->urls();
  QUrl u;
  QStringList failed;
  auto* pki = new pki_multi();

  foreach (u, urls)
    pki->probeAnything(u.toLocalFile());

  failed << pki->failed_files;
  importError(failed);
  addItem(pki);
  event->acceptProposedAction();
}

void ImportMulti::on_butRemove_clicked() {
  QItemSelectionModel* selectionModel = listView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedIndexes();
  QModelIndex index;
  QString items;

  foreach (index, indexes) {
    if (index.column() != 0) continue;
    mcont->remFromCont(index);
    pki_base* pki = db_base::fromIndex(index);
    delete pki;
  }
  if (mcont->rowCount(QModelIndex()) == 0) accept();
}

void ImportMulti::on_butOk_clicked() {
  if (!openDB()) return;

  Transaction;
  if (!TransBegin()) return;

  while (mcont->rowCount(QModelIndex()))
    import(mcont->index(0, 0, QModelIndex()));

  TransCommit();
  accept();
}

void ImportMulti::on_butImport_clicked() {
  QItemSelectionModel* selectionModel = listView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedIndexes();

  if (!openDB()) return;

  Transaction;
  if (!TransBegin()) return;
  foreach (QModelIndex index, indexes) {
    if (index.column() != 0) continue;
    import(index);
  }
  TransCommit();
  if (mcont->rowCount(QModelIndex()) == 0) accept();
}

void ImportMulti::on_deleteToken_clicked() {
  QItemSelectionModel* selectionModel = listView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedIndexes();
  QModelIndex index;
  QString items;

  foreach (index, indexes) {
    if (index.column() != 0) continue;
    pki_base* pki = db_base::fromIndex(index);
    try {
      pki->deleteFromToken(slot);
      mcont->remFromCont(index);
      delete pki;
    } catch (errorEx& err) {
      XCA_ERROR(err);
    }
  }
}
void ImportMulti::on_renameToken_clicked() {
  QItemSelectionModel* selectionModel = listView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedIndexes();
  QModelIndex index;
  QString items;

  foreach (index, indexes) {
    if (index.column() != 0) continue;
    listView->edit(index);
    break;
  }
}

pki_base* ImportMulti::import(const QModelIndex& idx) {
  pki_base* pki = mcont->fromIndex(idx);

  for (int i = 0; i < mcont->rowCount(idx); i++)
    import(mcont->index(i, 0, idx));

  if (!pki) return nullptr;

  mcont->remFromCont(idx);

  if (!Database.isOpen()) {
    delete pki;
    return nullptr;
  }
  return Database.insert(pki);
}

void ImportMulti::on_butDetails_clicked() {
  QItemSelectionModel* selectionModel = listView->selectionModel();
  QModelIndex index;

  if (!selectionModel->selectedIndexes().count()) return;

  index = selectionModel->selectedIndexes().first();
  pki_base* pki = db_base::fromIndex(index);

  if (!pki) return;
  try {
    auto* pki_super = dynamic_cast<pki_x509super*>(pki);
    if (pki_super) {
      CertDetail::showCert(this, pki_super);
      return;
    }
    auto* key = dynamic_cast<pki_key*>(pki);
    if (key) {
      KeyDetail::showKey(this, key);
      return;
    }
    auto* crl = dynamic_cast<pki_crl*>(pki);
    if (crl) {
      CrlDetail::showCrl(this, crl);
      return;
    }
    auto* temp = dynamic_cast<pki_temp*>(pki);
    if (temp) {
      XCA_WARN(
          tr("Details of the item '%1' cannot be shown").arg("XCA template"));
      return;
    }
    XCA_WARN(tr("The type of the item '%1' is not recognized")
                 .arg(pki->getClassName()));
  } catch (errorEx& err) {
    XCA_ERROR(err);
  }
}

ImportMulti::~ImportMulti() {
  QModelIndex idx = listView->currentIndex();
  while (idx != QModelIndex()) {
    mcont->remFromCont(idx);
    delete db_base::fromIndex(idx);
    idx = listView->currentIndex();
  }
  listView->setModel(nullptr);
  delete mcont;
}

int ImportMulti::entries() { return mcont->allItemsCount(); }

void ImportMulti::importError(QStringList failed) {
  if (failed.count() == 1) {
    XCA_INFO(tr("The file '%1' did not contain PKI data").arg(failed[0]));
  } else if (failed.count() > 1) {
    XCA_INFO(tr("The %1 files: '%2' did not contain PKI data")
                 .arg(failed.count())
                 .arg(failed.join("', '")));
  }
}

void ImportMulti::execute(int force, QStringList failed) {
  importError(failed);

  /* if there is nothing to import don't pop up */
  if (entries() == 0) {
    accept();
    return;
  }
  /* if there is only 1 item and force is 0 import it silently */
  if (entries() == 1 && force == 0 && openDB()) {
    QModelIndex idx = mcont->index(0, 0, QModelIndex());
    pki_base* pki = import(idx);
    if (pki && !Settings["suppress_messages"])
      XCA_INFO(pki->getMsg(pki_base::msg_import).arg(pki->getIntName()));
    accept();
    return;
  }
  /* the behaviour for more than one item */
  exec();
}