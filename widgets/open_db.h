#ifndef OPEN_DB_H
#define OPEN_DB_H

#include "lib/database_model.h"

#include <QDialog>
#include <QSqlDatabase>
#include "ui_open_db.h"

class OpenDb final : public QDialog, public Ui::OpenDb {
  Q_OBJECT
 private:
  static DbMap getDatabases();
  static QString lastRemote;
  bool sqlite, show_connection_settings;
  void setupDatabaseName(const QString& db);
  [[nodiscard]] QString getDbType() const;
  void fillDbDropDown(const QString& current);

 public:
  OpenDb(QWidget* parent, const QString& db);
  [[nodiscard]] QString getDescriptor() const;
  static bool hasSqLite();
  static void checkSqLite();
  static bool hasRemoteDrivers();
  static void setLastRemote(const QString& db);

 public slots:
  int exec() final;
  void driver_selected();
};

#endif  // OPEN_DB_H
