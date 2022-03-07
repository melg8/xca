#include "xca_tree_view.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMenu>
#include <QRegExp>
#include <QVariant>

#include "export_dialog.h"
#include "import_multi.h"
#include "lib/load_obj.h"
#include "main_window.h"
#include "oid_resolver.h"
#include "xca_application.h"
#include "xca_dialog.h"
#include "xca_header_view.h"
#include "xca_proxy_model.h"
#include "xca_warning.h"

#include "ui_item_properties.h"

XcaTreeView::XcaTreeView(QWidget* parent) : QTreeView(parent) {
  mainwin = nullptr;
  setHeader(new XcaHeaderView());
  setAlternatingRowColors(true);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setEditTriggers(QAbstractItemView::EditKeyPressed);
  setRootIsDecorated(false);
  setUniformRowHeights(true);
  setDragEnabled(true);
  // setAnimated(true);

  proxy = new XcaProxyModel(this);
  setSortingEnabled(true);
  proxy->setDynamicSortFilter(true);
  sortByColumn(0, Qt::AscendingOrder);
  basemodel = nullptr;
  connect(header(), SIGNAL(sectionHandleDoubleClicked(int)), this,
          SLOT(resizeColumnToContents(int)));
  connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this,
          SLOT(doubleClick(const QModelIndex&)));
  header()->setSectionsClickable(true);
  throttle.setSingleShot(true);
  connect(&throttle, SIGNAL(timeout()), this, SLOT(columnsResize()));
  connect(&throttle, SIGNAL(timeout()), proxy, SLOT(invalidate()));
  setFocusPolicy(Qt::StrongFocus);
  setExpandsOnDoubleClick(false);

  setFont(XcaApplication::tableFont);
}

XcaTreeView::~XcaTreeView() { delete proxy; }

void XcaTreeView::contextMenuEvent(QContextMenuEvent* e) {
  QModelIndex index;
  if (!basemodel) return;
  index = indexAt(e->pos());
  showContextMenu(e, getIndex(index));
}

void XcaTreeView::showHideSections() {
  if (!basemodel) return;
  int i, max = basemodel->columnCount(QModelIndex());
  basemodel->colResizeStart();
  for (i = 0; i < max; i++) {
    if (basemodel->columnHidden(i))
      header()->hideSection(i);
    else
      header()->showSection(i);
  }
  basemodel->colResizeEnd();
  columnsResize();
}

void XcaTreeView::setMainwin(MainWindow* mw, QLineEdit* filter) {
  mainwin = mw;
  connect(filter, SIGNAL(textChanged(const QString&)), this,
          SLOT(setFilter(const QString&)));
}

void XcaTreeView::setModel(QAbstractItemModel* model) {
  QByteArray ba;

  basemodel = dynamic_cast<db_base*>(model);
  proxy->setSourceModel(model);
  QTreeView::setModel(model ? proxy : nullptr);

  if (basemodel) {
    setRootIsDecorated(basemodel->treeViewMode());
    connect(basemodel, SIGNAL(resetHeader()), header(), SLOT(resetMoves()));
    connect(basemodel, SIGNAL(resetHeader()), this, SLOT(columnsResize()));
    connect(header(), SIGNAL(sectionMoved(int, int, int)), this,
            SLOT(sectionMoved(int, int, int)));
    connect(header(), SIGNAL(sectionResized(int, int, int)), basemodel,
            SLOT(sectionResized(int, int, int)));
    connect(header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
            basemodel, SLOT(sortIndicatorChanged(int, Qt::SortOrder)));
    connect(basemodel, SIGNAL(columnsContentChanged()), this,
            SLOT(columnsChanged()));

    basemodel->initHeaderView(header());
  }
  showHideSections();
}

void XcaTreeView::headerEvent(QContextMenuEvent* e, int col) {
  contextMenu(e, nullptr, col);
}

QModelIndex XcaTreeView::getIndex(const QModelIndex& index) {
  return proxy->mapToSource(index);
}

QModelIndex XcaTreeView::getProxyIndex(const QModelIndex& index) {
  return proxy->mapFromSource(index);
}

QModelIndexList XcaTreeView::getSelectedIndexes() {
  QModelIndexList list;
  QItemSelection indexes = selectionModel()->selection();
  list = proxy->mapSelectionToSource(indexes).indexes();

  /* Reduce list to column 0 items */
  QModelIndexList::iterator it = list.begin();
  while (it != list.end()) {
    if ((*it).column() != 0)
      it = list.erase(it);
    else
      ++it;
  }
  return list;
}

void XcaTreeView::columnsChanged() { throttle.start(200); }

void XcaTreeView::columnsResize() {
  int cnt, i;
  if (!basemodel) return;
  cnt = basemodel->columnCount(QModelIndex());
  basemodel->colResizeStart();
  for (i = 0; i < cnt; i++) {
    if (!basemodel->fixedHeaderSize(i)) {
      resizeColumnToContents(i);
    }
  }
  basemodel->colResizeEnd();
}

void XcaTreeView::sectionMoved(int, int, int) {
  int cnt = header()->count();
  for (int i = 0; i < cnt; i++) {
    basemodel->setVisualIndex(i, header()->visualIndex(i));
  }
}

QModelIndex XcaTreeView::currentIndex() {
  QModelIndex idx = QTreeView::currentIndex();
  idx = getIndex(idx);
  idx = basemodel->index(idx.row(), 0, idx.parent());
  if (!idx.isValid()) {
    QModelIndexList l = getSelectedIndexes();
    if (l.size() > 0) idx = l[0];
  }
  return idx;
}

void XcaTreeView::editIdx() { edit(getProxyIndex(currentIndex())); }

void XcaTreeView::setFilter(const QString& pattern) {
  pki_base::limitPattern =
      QRegExp(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
  // Only to tell the model about the changed filter
  proxy->setFilterFixedString(pattern);
}

void XcaTreeView::deleteItems() {
  QModelIndex index;
  QModelIndexList indexes = getSelectedIndexes();
  QString items, msg;
  int count = 0;
  pki_base* pki = nullptr;

  if (indexes.count() == 0 || !basemodel) return;

  foreach (index, indexes) {
    pki = db_base::fromIndex(index);
    items += "'" + pki->getIntName() + "' ";
    count++;
  }

  Transaction;
  if (!TransBegin()) return;

  if (count == 1)
    msg = pki->getMsg(pki_base::msg_delete).arg(pki->getIntName());
  else
    msg = pki->getMsg(pki_base::msg_delete_multi).arg(count).arg(items);

  if (!XCA_OKCANCEL(msg)) return;

  foreach (index, indexes) { basemodel->deletePKI(index); }
  TransCommit();
}

void XcaTreeView::exportItems() { exportItems(getSelectedIndexes()); }

void XcaTreeView::showItems() {
  QModelIndexList indexes = getSelectedIndexes();
  foreach (QModelIndex index, indexes)
    showItem(index);
}

void XcaTreeView::newItem() {
  if (basemodel) basemodel->newItem();
}

void XcaTreeView::load_default(load_base* load) {
  QString s;
  QStringList slist = QFileDialog::getOpenFileNames(
      nullptr, load->caption, Settings["workingdir"], load->filter);

  if (!slist.count()) return;

  update_workingdir(slist[0]);

  auto* dlgi = new ImportMulti(nullptr);
  foreach (s, slist) {
    pki_base* item = nullptr;
    try {
      item = load->loadItem(s);
      dlgi->addItem(item);
    } catch (errorEx& err) {
      XCA_ERROR(err);
      delete item;
    }
  }
  dlgi->execute();
  delete dlgi;
}

void XcaTreeView::doubleClick(const QModelIndex& m) { showItem(getIndex(m)); }

void XcaTreeView::editComment() {
  pki_base* item = db_base::fromIndex(currentIndex());
  if (!basemodel || !item) return;

  auto* w = new QWidget(nullptr);
  auto* prop = new Ui::ItemProperties();
  prop->setupUi(w);
  prop->comment->setPlainText(item->getComment());
  prop->name->setText(item->getIntName());
  prop->source->setText(item->pki_source_name());
  prop->insertionDate->setText(item->getInsertionDate().toPretty());
  auto* d = new XcaDialog(this, item->getType(), w, tr("Item properties"),
                          QString(), "itemproperties");
  if (d->exec())
    basemodel->updateItem(item, prop->name->text(),
                          prop->comment->toPlainText());
  delete d;
}

void XcaTreeView::pem2clipboard() {
  if (!basemodel) return;

  QString msg = basemodel->pem2QString(getSelectedIndexes());
  QClipboard* cb = QApplication::clipboard();

  if (cb->supportsSelection()) cb->setText(msg, QClipboard::Selection);
  cb->setText(msg);
}

void XcaTreeView::headerDetails() {
  if (curr_hd && curr_hd->id > 0 && mainwin)
    mainwin->getResolver()->searchOid(QString::number(curr_hd->id));
}

void XcaTreeView::columnRemove() {
  if (curr_hd->action) curr_hd->action->setChecked(false);
}

void XcaTreeView::showItem(const QModelIndex& index) {
  pki_base* pki = db_base::fromIndex(index);
  showItem(pki);
}

void XcaTreeView::showItem(const QString& name) {
  auto* pki = Store.lookupPki<pki_base>(name.toULongLong());
  showItem(pki);
}

void XcaTreeView::showItem(pki_base* pki) {
  if (pki && pki->isVisible() == 1) showPki(pki);
}

static void addSubmenu(tipMenu* menu, tipMenu* sub) {
  if (sub->isEmpty())
    delete sub;
  else
    menu->addMenu(sub);
}

void XcaTreeView::contextMenu(QContextMenuEvent* e, QMenu* parent, int col) {
  int shown = 0;
  tipMenu *menu, *dn, *v3ext, *current, *v3ns, *keyprop;
  QAction* a;
  dbheader* hd;
  dbheaderList allHeaders = basemodel->getAllHeaders();

  menu = new tipMenu(QString(), mainwin);
  dn = new tipMenu(tr("Subject entries"), mainwin);
  v3ext = new tipMenu(tr("X509v3 Extensions"), mainwin);
  v3ns = new tipMenu(tr("Netscape extensions"), mainwin);
  keyprop = new tipMenu(tr("Key properties"), mainwin);
  menu->addAction(tr("Reset"), basemodel, SLOT(columnResetDefaults()));
  if (col >= 0 && col < allHeaders.size()) {
    curr_hd = allHeaders[col];
    menu->addAction(tr("Hide Column"), this, SLOT(columnRemove()));
    if (curr_hd->id > 0)
      menu->addAction(tr("Details"), this, SLOT(headerDetails()));
  } else {
    curr_hd = nullptr;
  }
  menu->addSeparator();
  foreach (hd, allHeaders) {
    switch (hd->type) {
      case dbheader::hd_x509name:
        current = dn;
        break;
      case dbheader::hd_v3ext:
        current = v3ext;
        break;
      case dbheader::hd_v3ext_ns:
        if (Settings["disable_netscape"]) continue;
        current = v3ns;
        break;
      case dbheader::hd_key:
        current = keyprop;
        break;
      default:
        current = menu;
        break;
    }
    a = current->addAction(hd->getName());
    a->setCheckable(true);
    a->setChecked(hd->show);
    a->setToolTip(hd->getTooltip());
    hd->action = a;
  }

  menu->addSeparator();

  addSubmenu(menu, dn);
  addSubmenu(v3ext, v3ns);
  addSubmenu(menu, v3ext);
  addSubmenu(menu, keyprop);

  if (parent) {
    parent->addSeparator();
    parent->addMenu(menu)->setText(tr("Columns"));
    parent->exec(e->globalPos());
  } else {
    menu->exec(e->globalPos());
  }
  foreach (hd, allHeaders) {
    if (hd->action) hd->show = hd->action->isChecked();
    shown += hd->show ? 1 : 0;
    hd->action = nullptr;
  }
  if (!shown) allHeaders[0]->show = true;
  delete menu;
  delete parent;
  showHideSections();
}

void XcaTreeView::changeView() {
  if (!basemodel) return;
  hide();
  basemodel->changeView();
  show();
}

void XcaTreeView::exportItems(const QModelIndexList& indexes) {
  if (!basemodel || indexes.empty()) return;

  ExportDialog* dlg = exportDialog(indexes);

  if (dlg && dlg->exec()) {
    try {
      const pki_export* xport = dlg->export_type();
      XFile file(dlg->filename->text());

      if (xport->match_all(F_PRIVATE))
        file.open_key();
      else
        file.open_write();

      basemodel->exportItems(indexes, xport, file);
    } catch (errorEx& err) {
      XCA_ERROR(err);
    }
  }
  delete dlg;
}

ExportDialog* XcaTreeView::exportDialog(const QModelIndexList&) {
  return nullptr;
}

void XcaTreeView::showContextMenu(QContextMenuEvent* e,
                                  const QModelIndex& idx) {
  auto* menu = new QMenu(mainwin);
  QMenu* subExport = nullptr;
  QModelIndexList indexes = getSelectedIndexes();
  QModelIndex index;

  index = idx.isValid() ? idx : currentIndex();
  menu->addAction(tr("New"), this, SLOT(newItem()));
  menu->addAction(tr("Import"), this, SLOT(load()));
  menu->addAction(tr("Paste PEM data"), mainwin, SLOT(pastePem()),
                  QKeySequence::Paste);

  if (indexes.size() == 1) {
    menu->addAction(tr("Rename"), this, SLOT(editIdx()));
    menu->addAction(tr("Properties"), this, SLOT(editComment()));
  }
  if (indexes.size() > 0) {
    menu->addAction(tr("Delete"), this, SLOT(deleteItems()),
                    QKeySequence::Delete);
    subExport = menu->addMenu(tr("Export"));
    subExport->addAction(tr("Clipboard"), this, SLOT(pem2clipboard()),
                         QKeySequence::Copy);
    subExport->addAction(tr("File"), this, SLOT(exportItems()),
                         QKeySequence::Save);
  }

  fillContextMenu(menu, subExport, index, indexes);

  contextMenu(e, menu, -1);
}

void XcaTreeView::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
      deleteItems();
      return;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (state() != QAbstractItemView::EditingState) showItems();
      return;
    case Qt::Key_F2:
      editIdx();
      return;
    case Qt::Key_Escape:
      clearSelection();
      return;
  }
  if (event->matches(QKeySequence::Save)) {
    exportItems();
    return;
  }
  if (event->matches(QKeySequence::Copy)) {
    pem2clipboard();
    return;
  }
  QTreeView::keyPressEvent(event);
}
