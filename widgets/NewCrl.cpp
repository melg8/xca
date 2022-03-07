#include "new_crl.h"

#include "lib/database_model.h"
#include "lib/db_crl.h"
#include "lib/pki_x_509.h"

#include "new_crl_widget.h"
#include "xca_dialog.h"

[[nodiscard]] static NewCrlOptions NewCrlOptionsFrom(
    const pki_x509* issuer) noexcept {
  return {issuer->getIntName(), issuer->getRefKey()->possibleHashNids(),
          issuer->hasExtension(NID_subject_alt_name)};
}

void NewCrl::newCrl(QWidget* parent, pki_x509* issuer) {
  crljob task(issuer);
  auto* widget = new NewCrlWidget(task.settings, NewCrlOptionsFrom(issuer));
  auto* dlg =
      new XcaDialog(parent, revocation, widget, QObject::tr("Create CRL"),
                    QString(), "crlgenerate");
  if (dlg->exec()) {
    auto* db = Database.model<db_crl>();
    if (db) {
      task.settings = widget->getCrlJob();
      db->newCrl(task);
    }
  }
  delete dlg;
}
