/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2010 - 2011 Christian Hohnstaedt.
 *
 * All rights reserved.
 */


#include "NewCrl.h"
#include "XcaDialog.h"
#include "validity.h"
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

NewCrl::NewCrl(const CrlJobSettings &j,
               const NewCrlOptions &options, QWidget *w)
	: QWidget(w ? w : mainwin), task(j)
{
	setupUi(this);
    dateBox->setTitle(options.title);
    validNumber->setText(QString::number(
                             task.lastUpdate.daysTo(task.nextUpdate)));
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

CrlJobSettings NewCrl::getCrlJob() const
{
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

void NewCrl::on_applyTime_clicked()
{
	nextUpdate->setDiff(lastUpdate, validNumber->text().toInt(),
					validRange->currentIndex());
}

NewCrl::~NewCrl()
{
	qDebug() << "NewCrl::~NewCrl() -- DELETED";
}

void NewCrl::newCrl(QWidget *parent, pki_x509 *issuer)
{
    crljob task(issuer);
    NewCrl *widget = new NewCrl(task.settings, NewCrlOptionsFrom(issuer));
	XcaDialog *dlg = new XcaDialog(parent, revocation, widget,
				tr("Create CRL"), QString(), "crlgenerate");
	if (dlg->exec()) {
		db_crl *db = Database.model<db_crl>();
        if (db) {
            task.settings = widget->getCrlJob();
            db->newCrl(task);
        }
	}
	delete dlg;
}
