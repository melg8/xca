#ifndef DATABASE_MODEL_H
#define DATABASE_MODEL_H

#include <QDebug>
#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QStringList>

#include "db_base.h"
#include "lib/passwd.h"
#include "sql.h"

using DbMap = QMap<QString, QString>;

class database_model final : public QObject {
  Q_OBJECT

 private:
  QList<db_base*> models;
  int dbTimer;
  void openSqlDB();
  QSqlError initSqlDB();
  QString dbName;
  [[nodiscard]] bool checkForOldDbFormat(const QString& dbfile) const;
  [[nodiscard]] enum open_result verifyOldDbPass(const QString& dbname) const;
  void importOldDatabase(const QString& dbfile);
  [[nodiscard]] QString get_default_db() const;
  [[nodiscard]] QString checkPre2Xdatabase() const;
  [[nodiscard]] enum open_result initPass(const QString& db_name,
                                          const QString& passhash) const;
  void restart_timer();
  static void openDatabase(const QString& descriptor, const Passwd& pass);
  static void openRemoteDatabase(const QString& connName,
                                 const DbMap& params,
                                 const Passwd& pass);
  static void openLocalDatabase(const QString& connName,
                                const QString& descriptor);
 private slots:
  void pkiChangedSlot(pki_base* pki);

 public:
  static bool open_without_password;
  database_model(const QString& dbName, const Passwd& pass = Passwd());
  ~database_model() final;
  void timerEvent(QTimerEvent* event) final;
  db_base* modelForPki(const pki_base* pki) const;

  [[nodiscard]] QString dbname() const { return dbName; }
  void dump_database(const QString& dirname) const;
  [[nodiscard]] QList<db_base*> getModels() const { return models; }
  template <class T>
  T* model() const {
    foreach (db_base* model, models) {
      T* m = dynamic_cast<T*>(model);
      if (m) return m;
    }
    return nullptr;
  }
  pki_base* insert(pki_base* pki);

  static DbMap splitRemoteDbName(const QString& db);
  static bool isRemoteDB(const QString& db);
  static void as_default_database(const QString& db);

 signals:
  void pkiChanged(pki_base* pki) const;
};

class xca_db {
 private:
  database_model* db;

 public:
  xca_db() : db(nullptr) {}
  ~xca_db() { close(); }
  void open(const QString& dbName, const Passwd& pass = Passwd()) {
    close();
    db = new database_model(dbName, pass);
    qDebug() << "Opening database:" << name();
  }
  void open_default() { open(QString()); }
  void close() {
    if (db) {
      qDebug() << "Closing database:" << name();
      delete db;
      db = nullptr;
    }
  }
  [[nodiscard]] QString name() const { return db ? db->dbname() : QString(); }
  bool isOpen() { return db != nullptr; }
  template <class T>
  T* model() const {
    return db ? db->model<T>() : nullptr;
  }
  void dump(const QString& dirname) const {
    if (db) db->dump_database(dirname);
  }
  void as_default() const { database_model::as_default_database(name()); }
  [[nodiscard]] QList<db_base*> getModels() const {
    return db ? db->getModels() : QList<db_base*>();
  }
  pki_base* insert(pki_base* pki) { return db ? db->insert(pki) : nullptr; }
  db_base* modelForPki(const pki_base* pki) const {
    return db ? db->modelForPki(pki) : nullptr;
  }
  void connectToDbChangeEvt(QObject* o, const char* slot) {
    if (db) QObject::connect(db, SIGNAL(pkiChanged(pki_base*)), o, slot);
  }
};

extern xca_db Database;

#endif  // DATABASE_MODEL_H
