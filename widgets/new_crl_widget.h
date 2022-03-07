#ifndef NEW_CRL_WIDGET_H
#define NEW_CRL_WIDGET_H

#include <QWidget>

#include "ui_new_crl.h"

#include "lib/crl_job_settings.h"

struct NewCrlOptions {
  QString title = {};
  QList<int> possible_hash_nids = {};
  bool has_sub_alt_name = false;
};

class NewCrlWidget final : public QWidget, public Ui::NewCrl {
  Q_OBJECT

  CrlJobSettings task;

 public:
  NewCrlWidget(const CrlJobSettings& task,
               const NewCrlOptions& options,
               QWidget* w = nullptr);
  ~NewCrlWidget() final;
  [[nodiscard]] CrlJobSettings getCrlJob() const;

 public slots:
  void on_applyTime_clicked();
};

#endif  // NEW_CRL_WIDGET_H
