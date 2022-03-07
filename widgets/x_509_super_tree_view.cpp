#include "x_509_super_tree_view.h"

#include "cert_detail.h"
#include "lib/database_model.h"
#include "lib/db_x_509_super.h"
#include "main_window.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QFileDialog>
#include <QMenu>

void X509SuperTreeView::fillContextMenu(QMenu* menu,
                                        QMenu* subExport,
                                        const QModelIndex& index,
                                        QModelIndexList indexes) {
  pki_x509super* x = dynamic_cast<pki_x509super*>(db_base::fromIndex(index));
  transform = nullptr;

  if (indexes.size() != 1 || !x) return;

  subExport->addAction(tr("OpenSSL config"), this, SLOT(toOpenssl()));
  transform = menu->addMenu(tr("Transform"));
  transform->addAction(tr("Template"), this, SLOT(toTemplate()));
  transform->addAction(tr("Public key"), this, SLOT(extractPubkey()))
      ->setEnabled(!x->getRefKey());
}

void X509SuperTreeView::extractPubkey() {
  QModelIndex idx = currentIndex();

  if (idx.isValid() && basemodel) x509super()->extractPubkey(idx);
}

void X509SuperTreeView::toTemplate() {
  QModelIndex idx = currentIndex();

  if (idx.isValid() && basemodel) x509super()->toTemplate(idx);
}

void X509SuperTreeView::toOpenssl() {
  QModelIndex idx = currentIndex();

  if (!idx.isValid() || !basemodel) return;

  pki_x509super* pki = db_base::fromIndex<pki_x509super>(idx);
  QString fn = Settings["workingdir"] + pki->getUnderlinedName() + ".conf";
  QString fname = QFileDialog::getSaveFileName(
      nullptr, tr("Save as OpenSSL config"), fn,
      tr("Config files ( *.conf *.cnf);; All files ( * )"));
  if (fname.isEmpty()) return;

  update_workingdir(fname);
  pki->opensslConf(fname);
}

void X509SuperTreeView::showPki(pki_base* pki) {
  pki_x509super* x = dynamic_cast<pki_x509super*>(pki);
  CertDetail::showCert(this, x);
}
