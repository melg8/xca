#include "revocation_list.h"

#include "help.h"
#include "lib/asn_1_int.h"
#include "lib/pki_x_509.h"
#include "main_window.h"
#include "new_crl.h"

enum revCol { Cnumber, Cserial, Cdate, Creason, CiDate, Cmax };

class revListItem final : public QTreeWidgetItem {
 public:
  revListItem(QTreeWidget* w) : QTreeWidgetItem(w) {}
  bool operator<(const QTreeWidgetItem& other) const final {
    int col = treeWidget()->sortColumn();
    switch (col) {
      case Cserial: {
        return a1int(text(Cserial)) < a1int(other.text(Cserial));
      }
      case Cnumber:
        return text(Cnumber).toLong() < other.text(Cnumber).toLong();
      default:
        return QTreeWidgetItem::operator<(other);
    }
  }
};

static void setup_revRevItem(QTreeWidgetItem* item,
                             const x509rev& revit,
                             const pki_x509* iss) {
  pki_x509* rev = iss ? iss->getBySerial(revit.getSerial()) : nullptr;
  if (rev != nullptr) {
    for (int i = 0; i < Cmax; i++) item->setToolTip(i, rev->getIntName());
  }
  item->setText(Cserial, revit.getSerial());
  item->setText(Cdate, revit.getDate().toSortable());
  item->setText(Creason, revit.getReason());

  item->setTextAlignment(Cnumber, Qt::AlignRight);
  item->setTextAlignment(Cserial, Qt::AlignRight);

  a1time a = revit.getInvalDate();
  if (!a.isUndefined()) item->setText(CiDate, a.toSortable());
}

static void addRevItem(QTreeWidget* certList,
                       const x509rev& revit,
                       int no,
                       const pki_x509* iss) {
  revListItem* current;
  current = new revListItem(certList);
  current->setText(Cnumber, QString("%1").arg(no));
  setup_revRevItem(current, revit, iss);
}

void RevocationList::setupRevocationView(QTreeWidget* certsWidget,
                                         const x509revList& revList,
                                         const pki_x509* iss) {
  QStringList sl;
  int cols, i;

  certsWidget->clear();

  sl << tr("No.") << tr("Serial") << tr("Revocation") << tr("Reason")
     << tr("Invalidation");

  cols = sl.size();
  certsWidget->setColumnCount(cols);
  certsWidget->setHeaderLabels(sl);
  certsWidget->setItemsExpandable(false);
  certsWidget->setRootIsDecorated(false);
  certsWidget->sortItems(Cnumber, Qt::AscendingOrder);

  i = 1;
  foreach (x509rev revit, revList) { addRevItem(certsWidget, revit, i++, iss); }
  for (i = 0; i < cols; i++) certsWidget->resizeColumnToContents(i);
  certsWidget->setSortingEnabled(true);
  certsWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  certsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

RevocationList::RevocationList(QWidget* w) : QDialog(w ? w : mainwin) {
  QPushButton* genCrl;
  setupUi(this);
  setWindowTitle(XCA_TITLE);
  image->setPixmap(QPixmap(":revImg"));
  mainwin->helpdlg->register_ctxhelp_button(this, "crlmanage");

  genCrl =
      buttonBox->addButton(tr("Generate CRL"), QDialogButtonBox::ActionRole);

  connect(genCrl, SIGNAL(clicked()), this, SLOT(gencrl()));
}

void RevocationList::gencrl() {
  issuer->setRevocations(getRevList());
  NewCrl::newCrl(this, issuer);
}

void RevocationList::setRevList(const x509revList& rl, pki_x509* iss) {
  issuer = iss;
  revList = rl;
  setupRevocationView(certList, revList, issuer);
}

const x509revList& RevocationList::getRevList() { return revList; }

void RevocationList::on_addRev_clicked() {
  auto* revoke = new Revocation(QModelIndexList(), this);
  if (revoke->exec()) {
    x509rev revit = revoke->getRevocation();
    revList << revit;
    addRevItem(certList, revit, revList.size(), issuer);
  }
}

void RevocationList::on_delRev_clicked() {
  QTreeWidgetItem* current = certList->currentItem();
  x509rev rev;
  int idx;

  if (!current) return;
  idx = certList->indexOfTopLevelItem(current);
  certList->takeTopLevelItem(idx);
  rev.setSerial(a1int(current->text(Cserial)));
  idx = revList.indexOf(rev);
  if (idx != -1) revList.takeAt(idx);
}

void RevocationList::on_editRev_clicked() {
  on_certList_itemDoubleClicked(certList->currentItem());
}

void RevocationList::on_certList_itemDoubleClicked(QTreeWidgetItem* current) {
  x509rev rev;
  int idx;

  if (!current) return;

  rev.setSerial(a1int(current->text(Cserial)));
  idx = revList.indexOf(rev);
  if (idx == -1) return;

  rev = revList[idx];

  auto* revoke = new Revocation(QModelIndexList(), this);
  revoke->setRevocation(rev);
  if (revoke->exec()) {
    a1time a1 = rev.getDate();
    rev = revoke->getRevocation();
    rev.setDate(a1);
    revList[idx] = rev;
    setup_revRevItem(current, rev, issuer);
  }
  delete revoke;
}

Revocation::Revocation(QModelIndexList indexes, QWidget* w)
    : QDialog(w ? w : mainwin) {
  setupUi(this);
  setWindowTitle(XCA_TITLE);
  mainwin->helpdlg->register_ctxhelp_button(this, "crlrevocation");

  reason->addItems(x509rev::crlreasons());
  invalid->setNow();

  if (indexes.size() > 1) {
    QList<a1int> serials;
    QStringList sl;
    serial->setText(
        QString("Batch revocation of %1 Certificates").arg(indexes.size()));
    foreach (QModelIndex idx, indexes) {
      auto* cert = db_base::fromIndex<pki_x509>(idx);
      if (cert) serials << cert->getSerial();
    }
    std::sort(serials.begin(), serials.end());
    foreach (a1int a, serials)
      sl << a;
    serial->setToolTip(sl.join("\n"));
    serial->setEnabled(false);
  } else if (indexes.size() == 1) {
    auto* cert = db_base::fromIndex<pki_x509>(indexes[0]);
    serial->setText(cert->getSerial());
    serial->setEnabled(false);
  } else {
    serial->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]+"), serial));
  }
}

x509rev Revocation::getRevocation() {
  x509rev r;

  r.setSerial(a1int(serial->text()));
  r.setInvalDate(invalid->getDate());
  r.setDate(a1time());
  r.setCrlNo(0);
  r.setReason(reason->currentText());
  return r;
}

void Revocation::setRevocation(x509rev r) {
  serial->setText(r.getSerial());
  invalid->setDate(r.getInvalDate());
  int i = reason->findText(r.getReason());
  if (i == -1) i = 0;
  reason->setCurrentIndex(i);
}