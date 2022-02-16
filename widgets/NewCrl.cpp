/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2010 - 2011 Christian Hohnstaedt.
 *
 * All rights reserved.
 */


#include "NewCrl.h"
#include "XcaDialog.h"
#include "validity.h"
#include "new_crl_widget.h"
#include "MainWindow.h"
#include "lib/base.h"
#include "lib/func.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>

[[nodiscard]] static NewCrlOptions NewCrlOptionsFrom(
        const pki_x509 *issuer) noexcept {
    return {
       issuer->getIntName(),
       issuer->getRefKey()->possibleHashNids(),
       issuer->hasExtension(NID_subject_alt_name)
    };
}

void NewCrl::newCrl(QWidget *parent, pki_x509 *issuer)
{
    crljob task(issuer);
    NewCrlWidget *widget = new NewCrlWidget(task.settings, NewCrlOptionsFrom(issuer));
	XcaDialog *dlg = new XcaDialog(parent, revocation, widget,
                QObject::tr("Create CRL"), QString(), "crlgenerate");
	if (dlg->exec()) {
		db_crl *db = Database.model<db_crl>();
        if (db) {
            task.settings = widget->getCrlJob();
            db->newCrl(task);
        }
	}
	delete dlg;
}
