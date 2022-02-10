/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2018 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#ifndef __IPVALIDATOR_H
#define __IPVALIDATOR_H

#include <QStringList>
#include <QRegExp>
#include <QString>
#include <QValidator>

/* Validating IPv4/6 is not as trivial as thought.
 * - The QHostAddress class requires the network library
 *   and i don't want to add many megabytes for this validator
 * - I failed to and actually don't want to write my own validator
 *   as there are already thoroughly tested functions.
 * - inet_pton() does what I want, but
 *   on Windows there is no inet_pton().
 * - Openssl validates the result anyway.
 *   So we reduce the validation on windows to the RegEx
 */
#if !defined(Q_OS_WIN32)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

[[nodiscard]] static bool IsValidIp(const QString &input) noexcept {
    unsigned char buf[sizeof(struct in6_addr)];
    return inet_pton(AF_INET, CCHAR(input), buf) == 1 ||
           inet_pton(AF_INET6, CCHAR(input), buf) == 1;
}
#else
[[nodiscard]] static bool IsValidIp(const QString &input) noexcept {
    return true;
}
#endif

class ipValidator : public QValidator
{
    public:
	QValidator::State validate(QString &input, int&) const
	{
		if (!QRegExp("[0-9a-fA-F:\\.]*").exactMatch(input))
            return Invalid;
        return IsValidIp(input) ? Acceptable : Intermediate;
	}
	void fixup(QString &input) const
	{
		input = input.toLower();
	}
};
#endif
