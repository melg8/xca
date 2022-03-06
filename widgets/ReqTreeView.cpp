#include "ReqTreeView.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMenu>
#include "ExportDialog.h"
#include "MainWindow.h"
#include "lib/load_obj.h"
#include "lib/pki_x509req.h"

void ReqTreeView::fillContextMenu(QMenu* menu,
                                  QMenu* subExport,
                                  const QModelIndex& index,
                                  QModelIndexList indexes) {
  X509SuperTreeView::fillContextMenu(menu, subExport, index, indexes);

  pki_x509req* req = db_base::fromIndex<pki_x509req>(index);

  if (indexes.size() != 1 || !req) return;

  menu->addAction(tr("Sign"), this, SLOT(signReq()));
  if (req->getDone())
    menu->addAction(tr("Unmark signed"), this, SLOT(unmarkSigned()));
  else
    menu->addAction(tr("Mark signed"), this, SLOT(markSigned()));
  if (transform) {
    transform->addAction(tr("Similar Request"), this, SLOT(toRequest()));
  }
}

void ReqTreeView::signReq() {
  pki_x509req* req = db_base::fromIndex<pki_x509req>(currentIndex());
  db_x509* certs = Database.model<db_x509>();
  certs->newCert(req);
}

void ReqTreeView::toRequest() {
  pki_x509req* req = db_base::fromIndex<pki_x509req>(currentIndex());
  if (basemodel) reqs()->newItem(nullptr, req);
}

void ReqTreeView::markSigned() {
  if (basemodel) reqs()->setSigned(currentIndex(), true);
}

void ReqTreeView::unmarkSigned() {
  if (basemodel) reqs()->setSigned(currentIndex(), false);
}

void ReqTreeView::load(void) {
  load_req l;
  load_default(&l);
}

ExportDialog* ReqTreeView::exportDialog(const QModelIndexList& indexes) {
  return new ExportDialog(
      this, tr("Certificate request export"),
      tr("Certificate request ( *.pem *.der *.csr )"), indexes,
      QPixmap(":csrImg"),
      pki_export::select(x509_req, basemodel->exportFlags(indexes)),
      "csrexport");
}
