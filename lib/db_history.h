#ifndef DB_HISTORY_H
#define DB_HISTORY_H

#include <QString>
#include <QStringList>

class dbhistory {
 private:
  QStringList history;
  static QString lastRemote;

 public:
  dbhistory();
  void addEntry(const QString& name);
  QStringList get() const { return history; }
  static void setLastRemote(const QString& db);
  static QString getLastRemote();
};

#endif  // DB_HISTORY_H
