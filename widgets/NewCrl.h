/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2010 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#ifndef __NEWCRL_H
#define __NEWCRL_H

class pki_x509;
class QWidget;

class NewCrl {
   public:
    static void newCrl(QWidget *parent, pki_x509 *issuer);
};

#endif
