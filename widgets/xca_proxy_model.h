#ifndef XCA_PROXY_MODEL_H
#define XCA_PROXY_MODEL_H

#include <QSortFilterProxyModel>

#include <QItemSelectionModel>
#include <QWidget>

class XcaProxyModel final : public QSortFilterProxyModel {
  Q_OBJECT
 public:
  XcaProxyModel(QWidget* parent = nullptr) : QSortFilterProxyModel(parent) {}
  [[nodiscard]] bool lessThan(const QModelIndex& left,
                              const QModelIndex& right) const final;
  [[nodiscard]] bool filterAcceptsRow(
      int sourceRow, const QModelIndex& sourceParent) const final;
  [[nodiscard]] QVariant data(const QModelIndex& index, int role) const final;
};

#endif  // XCA_PROXY_MODEL_H
