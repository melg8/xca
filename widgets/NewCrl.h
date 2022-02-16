/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2010 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#ifndef __NEWCRL_H
#define __NEWCRL_H

#include "ui_NewCrl.h"
#include "lib/pki_crl.h"

class pki_key;

struct NewCrlOptions {
    QString title = {};
    QList<int> possible_hash_nids = {};
    bool has_sub_alt_name = false;
};

class NewCrlWidget final: public QWidget, public Ui::NewCrl
{
	Q_OBJECT

    CrlJobSettings task;
   public:
    NewCrlWidget(const CrlJobSettings &task,
           const NewCrlOptions &options, QWidget *w = nullptr);
    ~NewCrlWidget() final;
    CrlJobSettings getCrlJob() const;

   public slots:
	void on_applyTime_clicked();
};

class NewCrl {
   public:
    static void newCrl(QWidget *parent, pki_x509 *issuer);
};

#endif
