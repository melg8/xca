/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2001 - 2020 Christian Hohnstaedt.
 *
 * All rights reserved.
 */


#ifndef __PKI_MULTI_H
#define __PKI_MULTI_H

#include <QStringList>
#include "pki_base.h"

class pki_multi: public pki_base
{
		Q_OBJECT
	protected:
		QList<pki_base*> multi;
	public:
		pki_multi(const QString &name = "");
        ~pki_multi() override;
		QStringList failed_files;
        void fromPEMbyteArray(const QByteArray &, const QString &) override;
        void fload(const QString &fname) override;
		void probeAnything(const QString &fname);
		void append_item(pki_base *pki);
        void print(BioByteArray &b, enum print_opt opt) const override;
		QList<pki_base *> pull();
		QList<pki_base *> get() const;
};
#endif
