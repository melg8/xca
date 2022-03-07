#include "hash_box.h"

#include <QDebug>
#include "lib/base.h"

hashBox::hashBox(QWidget* parent) : QComboBox(parent) { setupAllHashes(); }

const digest hashBox::current() const { return digest(currentText()); }

void hashBox::setCurrent(const digest& md) {
  int idx = findText(md.name());
  if (idx != -1) {
    setCurrentIndex(idx);
    wanted_md = "";
  } else {
    wanted_md = md.name();
  }
}

void hashBox::setupHashes(QList<int> nids) {
  QString md = currentText();

  if (!wanted_md.isEmpty()) {
    md = wanted_md;
  }
  clear();
  foreach (int nid, digest::all_digests) {
    if (nids.contains(nid)) {
      addItem(digest(nid).name());
    }
  }
  setEnabled(count() > 0);
  setDefaultHash();
  if (!md.isEmpty()) {
    setCurrent(digest(md));
  } else {
    setDefaultHash();
  }
}

void hashBox::setupAllHashes() { setupHashes(digest::all_digests); }

void hashBox::setDefaultHash() { setCurrent(digest::getDefault()); }
