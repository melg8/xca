#ifndef XCA_HEADER_VIEW_H
#define XCA_HEADER_VIEW_H

#include <QHeaderView>

class XcaHeaderView final : public QHeaderView {
  Q_OBJECT

 public:
  XcaHeaderView();
  void contextMenuEvent(QContextMenuEvent* e) final;

 public slots:
  void resetMoves();
};

#endif  // XCA_HEADER_VIEW_H
