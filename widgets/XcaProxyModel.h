#ifndef __XCAPROXYMODEL_H
#define __XCAPROXYMODEL_H

#include <QSortFilterProxyModel>

#include <QItemSelectionModel>
#include <QWidget>

class XcaProxyModel final : public QSortFilterProxyModel {
  Q_OBJECT
 public:
  XcaProxyModel(QWidget* parent = nullptr) : QSortFilterProxyModel(parent) {}
  bool lessThan(const QModelIndex& left, const QModelIndex& right) const final;
  bool filterAcceptsRow(int sourceRow,
                        const QModelIndex& sourceParent) const final;
  QVariant data(const QModelIndex& index, int role) const final;
};

#endif
