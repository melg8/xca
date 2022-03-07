#ifndef PKI_EXPORT_H
#define PKI_EXPORT_H

#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>

#define BIT(n) (1 << n)
#define DEFAULT_KEY_CLIPBOARD_TYPE 19

enum pki_type {
  none,
  asym_key,
  x509_req,
  x509,
  revocation,
  tmpl,
  setting,
  smartCard,
};

enum {
  F_PEM = BIT(0),       /* File is PEM encoded */
  F_CRYPT = BIT(1),     /* Key will be encrypted */
  F_PRIVATE = BIT(2),   /* File contains Private Key */
  F_USUAL = BIT(3),     /* Usual format */
  F_SSH2 = BIT(4),      /* Key is SSH2 compatible */
  F_MULTI = BIT(5),     /* Hide if only element is selected */
  F_SINGLE = BIT(6),    /* Single element not suitable for selections */
  F_ALL = BIT(7),       /* All elements */
  F_UNREVOKED = BIT(8), /* All unrevoked certificates */
  F_PLUSKEY = BIT(9),   /* Cert plus key */
  F_CA = BIT(10),       /* Export types for CAs */
  F_CHAIN = BIT(11),    /* Certificate chain */
  F_PKCS7 = BIT(12),
  F_PKCS8 = BIT(13),
  F_PKCS12 = BIT(14),
  F_CLIPBOARD = BIT(15), /* Suitable key format for the clipboard */
  F_PVK = BIT(16),
  F_INDEX = BIT(17),
  F_CAL = BIT(18), /* Calendar entry */
  F_DER = BIT(19), /* DER format */
};

class pki_export : public QObject {
  Q_OBJECT

 public:
  int id;
  enum pki_type pki_type;
  QString extension;
  int flags;
  QString desc;
  QString help;

  pki_export(int i,
             enum pki_type p,
             const QString& e,
             const QString& d,
             int f,
             const QString& h);
  static QList<const pki_export*> select(enum pki_type, int);
  static const pki_export* by_id(int id);
  [[nodiscard]] bool match_all(int match_flags) const;

 private:
  static QList<pki_export*> elements;
};

#endif  // PKI_EXPORT_H
