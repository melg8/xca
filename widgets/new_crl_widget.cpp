#include "new_crl_widget.h"

#include <utility>

#include "main_window.h"

NewCrlWidget::NewCrlWidget(CrlJobSettings j,
                           const NewCrlOptions& options,
                           QWidget* w)
    : QWidget(w ? w : mainwin), task(std::move(j)) {
  setupUi(this);
  dateBox->setTitle(options.title);
  validNumber->setText(
      QString::number(task.lastUpdate.daysTo(task.nextUpdate)));
  validRange->setCurrentIndex(0);
  on_applyTime_clicked();
  nextUpdate->setEndDate(true);

  hashAlgo->setupHashes(options.possible_hash_nids);
  hashAlgo->setCurrent(task.hashAlgo);

  crlNumber->setText(task.crlNumber.toDec());
  if (options.has_sub_alt_name) {
    subAltName->setEnabled(true);
    subAltName->setChecked(task.subAltName);
  } else {
    subAltName->setEnabled(false);
  }
  revocationReasons->setChecked(task.withReason);
  authKeyId->setChecked(task.authKeyId);
}

CrlJobSettings NewCrlWidget::getCrlJob() const {
  CrlJobSettings t = task;
  t.withReason = revocationReasons->isChecked();
  t.authKeyId = authKeyId->isChecked();
  t.subAltName = subAltName->isChecked();
  t.setCrlNumber = setCrlNumber->isChecked();
  t.lastUpdate = lastUpdate->getDate();
  t.nextUpdate = nextUpdate->getDate();
  t.hashAlgo = hashAlgo->current();
  t.crlNumber = crlNumber->text().toLong();
  return t;
}

void NewCrlWidget::on_applyTime_clicked() {
  nextUpdate->setDiff(lastUpdate, validNumber->text().toInt(),
                      validRange->currentIndex());
}

NewCrlWidget::~NewCrlWidget() { qDebug() << "NewCrl::~NewCrl() -- DELETED"; }
