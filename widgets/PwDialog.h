/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2011 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#ifndef __PWDIALOG_H
#define __PWDIALOG_H

#include "ui_PwDialog.h"
#include "lib/PwDialogCore.h"
#include "lib/pass_info.h"

class PwDialog final: public QDialog, public Ui::PwDialog
{
	Q_OBJECT

   private:
	bool wrDialog;
	Passwd final;
	pass_info *pi;

   public:
	PwDialog(pass_info *p, bool write = false);
	Passwd getPass() {
		return final;
	}
	void addAbortButton();
	void setRW(bool write);

	enum open_result execute(pass_info *p, Passwd *passwd,
			bool write = false, bool abort = false);

   public slots:
    void accept() final;
	void buttonPress(QAbstractButton *but);
};

class PwDialogUI final: public PwDialogUI_i
{
	enum open_result execute(pass_info *p, Passwd *passwd,
                        bool write = false, bool abort = false) final;
};
#endif
