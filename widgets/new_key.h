#ifndef NEW_KEY_H
#define NEW_KEY_H

#include <QStringList>
#include "lib/builtin_curves.h"
#include "lib/pkcs_11_lib.h"
#include "lib/pki_key.h"
#include "ui_new_key.h"

class NewKey final : public QDialog, public Ui::NewKey {
  Q_OBJECT
 private:
  void updateCurves(unsigned min = 0,
                    unsigned max = INT_MAX,
                    unsigned long ec_flags = 0);
  void addCurveBoxCurves(const QList<builtin_curve>& curves);

 public:
  NewKey(QWidget* parent, const QString& name);
  [[nodiscard]] keyjob getKeyJob() const;

 public slots:
  void accept() final;
  void on_keyType_currentIndexChanged(int);
};

#endif  // NEW_KEY_H
