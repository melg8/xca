#ifndef PKI_LOOKUP_H
#define PKI_LOOKUP_H

#include <QDebug>
#include <QHash>
#include <QString>
#include <QVariant>
#include <typeinfo>

#include "xca_warning_core.h"

#include "base.h"
#include "sql.h"

class pki_base;
class pki_lookup {
 private:
  QHash<quint64, pki_base*> lookup;
  [[nodiscard]] pki_base* get(quint64 id) const {
    if (id > 0 && !lookup.keys().contains(id))
      qCritical("pki_lookup: ID %u not found", (unsigned)id);
    return lookup[id];
  }

 public:
  ~pki_lookup() { flush(); }
  void add(QVariant id, pki_base* pki) { add(id.toULongLong(), pki); }
  void add(quint64 id, pki_base* pki) {
    if (id == 0) qCritical("pki_lookup: ID 0 not permitted");
    if (!pki)
      qCritical(
          "pki_lookup: Refusing to add NULL item "
          "with ID %u",
          (unsigned)id);
    if (lookup.keys().contains(id))
      qCritical("pki_lookup: ID %u in use", (unsigned)id);
    lookup[id] = pki;
  }

  XSqlQuery sqlSELECTpki(QString query,
                         QList<QVariant> values = QList<QVariant>()) {
    XSqlQuery q;
    int i, num_values = values.size();

    SQL_PREPARE(q, query);
    for (i = 0; i < num_values; i++) q.bindValue(i, values[i]);
    q.exec();
    XCA_SQLERROR(q.lastError());
    return q;
  }

  template <class T>
  QList<T*> sqlSELECTpki(QString query,
                         QList<QVariant> values = QList<QVariant>()) {
    XSqlQuery q = sqlSELECTpki(query, values);
    QList<T*> x;
    while (q.next()) {
      T* pki = lookupPki<T>(q.value(0));
      if (pki) x << pki;
    }
    return x;
  }

  template <class T>
  T* lookupPki(QVariant v) const {
    quint64 id = v.toULongLong();
    if (id == 0) return nullptr;
    T* pki = dynamic_cast<T*>(get(id));
    if (pki) return pki;

    pki_base* p = get(id);
    qCritical() << QString("Invalid Type of ItemId(%1) %2 %3. Expected %4.")
                       .arg(id)
                       .arg(typeid(p).name())
                       .arg("")  // p ? p->getIntName() : "<NULL item>")
                       .arg(typeid(T*).name());
    return nullptr;
  }

  template <class T>
  QList<T*> getAll() const {
    QList<T*> result;
    foreach (pki_base* pki, lookup.values()) {
      T* p = dynamic_cast<T*>(pki);
      if (p) result << p;
    }
    return result;
  }
  pki_base* operator[](quint64 id) const { return get(id); }
  void flush() {
    qDeleteAll(lookup);
    lookup.clear();
  }
};

#endif  // PKI_LOOKUP_H
