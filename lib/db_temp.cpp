#include "db_temp.h"

#include <QDir>
#include <QFileInfo>
#include "func.h"
#include "load_obj.h"
#include "pki_temp.h"
#include "xca_warning_core.h"

db_temp::db_temp() : db_x509name("templates") {
  /* XCA loads templates from private space ($HOME/.local/)
   * Host specific (/usr/local) and distribution (/usr)
   * The first <name>.xca found avoids other <name>.xca to be loaded
   */
  QSet<QString> template_files;
  load_temp l;

  sqlHashTable = "templates";
  pkitype << tmpl;

  updateHeaders();
  loadContainer();

  auto* tmpl = new pki_temp(tr("Empty template"));
  tmpl->setAsPreDefined();
  predefs << tmpl;

  foreach (QString d,
           QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)) {
    QFileInfoList list = QDir(d).entryInfoList(
        QStringList("*.xca"),
        QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::Readable);

    foreach (QFileInfo fileInfo, list) {
      if (template_files.contains(fileInfo.fileName())) continue;

      qDebug() << "Loading template" << fileInfo.fileName()
               << fileInfo.absoluteFilePath();
      try {
        tmpl = dynamic_cast<pki_temp*>(l.loadItem(fileInfo.absoluteFilePath()));
        if (tmpl) {
          tmpl->setAsPreDefined();
          predefs << tmpl;
          template_files << fileInfo.fileName();
        }
      } catch (const errorEx&) {
        XCA_WARN(tr("Bad template: %1")
                     .arg(nativeSeparator(fileInfo.absoluteFilePath())));
      }
    }
  }
}

db_temp::~db_temp() { qDeleteAll(predefs); }

pki_base* db_temp::newPKI(enum pki_type type) {
  (void)type;
  return new pki_temp("");
}

QList<pki_temp*> db_temp::getPredefs() const { return predefs; }

bool db_temp::alterTemp(pki_temp* temp) {
  XSqlQuery q;
  QSqlError e;

  Transaction;
  if (!TransBegin()) return false;
  SQL_PREPARE(q, "UPDATE templates SET version=?, template=? WHERE item=?");
  q.bindValue(0, TMPL_VERSION);
  q.bindValue(1, temp->toB64Data());
  q.bindValue(2, temp->getSqlItemId());
  q.exec();
  e = q.lastError();
  XCA_SQLERROR(e);
  if (e.isValid()) {
    TransRollback();
    return false;
  }
  updateItem(temp, temp->getIntName(), temp->getComment());
  TransCommit();
  return true;
}

void db_temp::exportItem(const QModelIndex& index,
                         const pki_export*,
                         XFile& file) const {
  auto* temp = fromIndex<pki_temp>(index);
  if (temp) temp->writeTemp(file);
}
