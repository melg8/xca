/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2001 - 2007 Christian Hohnstaedt.
 *
 * All rights reserved.
 */


#ifndef PKI_MULTI_H
#define PKI_MULTI_H

#include <openssl/pem.h>
#include "pki_x509.h"
#include "x509name.h"
#include "asn1time.h"
#include "asn1int.h"

class pki_multi: public pki_base
{
	protected:
		QList<pki_base*> multi;
	public:
		pki_multi(const QString name = "");
		~pki_multi();
		void fload(const QString fname);
		pki_base *pull();
};

#endif