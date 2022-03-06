#include "TempTreeView.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMenu>
#include "ExportDialog.h"
#include "MainWindow.h"
#include "NewX509.h"
#include "XcaDialog.h"
#include "lib/load_obj.h"
#include "lib/pki_temp.h"

void TempTreeView::fillContextMenu(QMenu* menu,
                                   QMenu*,
                                   const QModelIndex&,
                                   QModelIndexList indexes) {
  if (indexes.size() != 1) return;

  menu->addAction(tr("Duplicate"), this, SLOT(duplicateTemp()));
  menu->addAction(tr("Create certificate"), this, SLOT(certFromTemp()));
  menu->addAction(tr("Create request"), this, SLOT(reqFromTemp()));
}

void TempTreeView::duplicateTemp() {
  pki_temp* temp = db_base::fromIndex<pki_temp>(currentIndex());
  db_temp* db_temp = temps();
  if (!temp || !basemodel || !db_temp) return;

  pki_temp* newtemp = new pki_temp(temp);
  newtemp->setIntName(newtemp->getIntName() + " " + tr("copy"));
  db_temp->insertPKI(newtemp);
}

void TempTreeView::certFromTemp() {
  pki_temp* temp = db_base::fromIndex<pki_temp>(currentIndex());
  if (temp) emit newCert(temp);
}

void TempTreeView::reqFromTemp() {
  pki_temp* temp = db_base::fromIndex<pki_temp>(currentIndex());
  if (temp) emit newReq(temp);
}

void TempTreeView::showPki(pki_base* pki) {
  alterTemp(dynamic_cast<pki_temp*>(pki));
}

bool TempTreeView::runTempDlg(pki_temp* temp) {
  NewX509* dlg = new NewX509(this);

  dlg->setTemp(temp);
  if (!dlg->exec()) {
    delete dlg;
    return false;
  }
  dlg->toTemplate(temp);
  delete dlg;
  return true;
}

void TempTreeView::newItem() {
  pki_temp* temp = nullptr;
  QString type;

  if (!basemodel) return;

  itemComboTemp* ic = new itemComboTemp(nullptr);
  ic->insertPkiItems(temps()->getPredefs());
  XcaDialog* dlg =
      new XcaDialog(this, tmpl, ic, tr("Preset Template values"), QString());
  if (dlg->exec()) {
    temp = new pki_temp(ic->currentPkiItem());
    if (temp) {
      temp->pkiSource = generated;
      if (runTempDlg(temp)) {
        temp = dynamic_cast<pki_temp*>(temps()->insertPKI(temp));
        temps()->createSuccess(temp);
      } else {
        delete temp;
      }
    }
  }
  delete dlg;
}

bool TempTreeView::alterTemp(pki_temp* temp) {
  if (!basemodel || !temp) return false;

  if (!runTempDlg(temp)) return false;

  temps()->alterTemp(temp);
  return true;
}

void TempTreeView::load() {
  load_temp l;
  load_default(&l);
}

ExportDialog* TempTreeView::exportDialog(const QModelIndexList& indexes) {
  return new ExportDialog(
      this, tr("Template export"), tr("XCA Templates ( *.xca )"), indexes,
      QPixmap(":tempImg"),
      pki_export::select(tmpl, basemodel->exportFlags(indexes)),
      "templateexport");
}
