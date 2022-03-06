#ifndef __IPVALIDATOR_H
#define __IPVALIDATOR_H

#include <QRegExp>
#include <QString>
#include <QStringList>
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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

[[nodiscard]] static bool IsValidIp(const QString& input) noexcept {
  unsigned char buf[sizeof(struct in6_addr)];
  return inet_pton(AF_INET, CCHAR(input), buf) == 1 ||
         inet_pton(AF_INET6, CCHAR(input), buf) == 1;
}
#else
[[nodiscard]] static bool IsValidIp(const QString&) noexcept { return true; }
#endif

class ipValidator final : public QValidator {
 public:
  QValidator::State validate(QString& input, int&) const final {
    if (!QRegExp("[0-9a-fA-F:\\.]*").exactMatch(input)) return Invalid;
    return IsValidIp(input) ? Acceptable : Intermediate;
  }
  void fixup(QString& input) const final { input = input.toLower(); }
};

#endif
