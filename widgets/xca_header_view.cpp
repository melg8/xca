#include "xca_header_view.h"

#include "xca_tree_view.h"

#include <QContextMenuEvent>

XcaHeaderView::XcaHeaderView() : QHeaderView(Qt::Horizontal) {
  setSectionsMovable(true);
}

void XcaHeaderView::contextMenuEvent(QContextMenuEvent* e) {
  auto* tv = static_cast<XcaTreeView*>(parentWidget());
  if (tv) {
    tv->headerEvent(e, logicalIndexAt(e->pos()));
  }
}

void XcaHeaderView::resetMoves() {
  for (int i = 0; i < count(); i++) {
    if (i != visualIndex(i)) {
      moveSection(visualIndex(i), i);
      i = 0;
    }
  }
  resizeSections(QHeaderView::ResizeToContents);
}
