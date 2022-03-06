#ifndef __XCAHEADERVIEW_H
#define __XCAHEADERVIEW_H

#include <QHeaderView>

class XcaHeaderView final : public QHeaderView {
  Q_OBJECT

 public:
  XcaHeaderView();
  void contextMenuEvent(QContextMenuEvent* e) final;

 public slots:
  void resetMoves();
};

#endif
