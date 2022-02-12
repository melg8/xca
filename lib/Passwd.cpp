/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2012 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#include <QByteArray>
#include "Passwd.h"

void Passwd::cleanse()
{
	memset(data(), 0, size());
	clear();
}

Passwd::~Passwd()
{
	Passwd::cleanse();
}
const unsigned char *Passwd::constUchar() const
{
    return reinterpret_cast<const unsigned char*>(size() ? constData() : "");
}
