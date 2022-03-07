#include "db_history.h"

#include <QDir>
#include <QString>

#include "database_model.h"
#include "func.h"
#include "x_file.h"

static QString dbhistory_file() { return getUserSettingsDir() + "/dbhistory"; }
QString dbhistory::lastRemote;

dbhistory::dbhistory() {
  QString name;
  XFile file(dbhistory_file());

  try {
    file.open_read();
  } catch (...) {
    return;
  }

  history.clear();
  while (!file.atEnd()) {
    QByteArray ba;
    ba = file.readLine(1024);
    if (ba.size() == 0) {
      break;
    }
    name = QString::fromUtf8(ba).trimmed();
    if (name.size() == 0) {
      continue;
    }
    if (history.indexOf(name) == -1) {
      history << name;
    }
  }
  file.close();

  foreach (name, history) {
    if (database_model::isRemoteDB(name)) {
      setLastRemote(name);
      break;
    }
  }
}

void dbhistory::addEntry(const QString& name) {
  int pos;
  QString fname = name;

  if (!database_model::isRemoteDB(fname)) {
    fname = relativePath(fname);
  }

  pos = history.indexOf(fname);
  if (pos == 0) {
    return; /* no changes */
  }

  if (pos > 0) {
    history.removeAt(pos);
  }

  history.prepend(fname);
  while (history.size() > 10) {
    history.removeLast();
  }

  XFile file(dbhistory_file());
  if (!file.open_write()) {
    return;
  }

  QString all = history.join("\n");
  if (file.write(all.toUtf8()) <= 0) {
    qDebug() << "Error writing history" << file.fileName()
             << file.errorString();
  }
  file.close();
}

void dbhistory::setLastRemote(const QString& db) {
  if (database_model::isRemoteDB(db)) {
    lastRemote = db;
  }
}

QString dbhistory::getLastRemote() { return lastRemote; }
