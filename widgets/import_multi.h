#ifndef IMPORT_MULTI_H
#define IMPORT_MULTI_H

#include <QDialog>
#include "ui_import_multi.h"

#include <QList>
#include "lib/db_base.h"
#include "lib/db_token.h"

class pki_x509;
class pki_key;

class ImportMulti final : public QDialog, private Ui::ImportMulti {
  Q_OBJECT

 private:
  slotid slot;
  db_token* mcont;
  void importError(QStringList failed);

 public:
  ImportMulti(QWidget* parent);
  ~ImportMulti() final;
  void addItem(pki_base* pki);
  pki_base* getSelected();
  pki_base* import(const QModelIndex& idx);
  void execute(int force = 0, QStringList failed = QStringList());
  int entries();
  void tokenInfo(const slotid& s);
  void dragEnterEvent(QDragEnterEvent* event) final;
  void dropEvent(QDropEvent* event) final;
  bool openDB() const;

 public slots:
  void on_butRemove_clicked();
  void on_butImport_clicked();
  void on_butDetails_clicked();
  void on_butOk_clicked();
  void on_deleteToken_clicked();
  void on_renameToken_clicked();
};

#endif  // IMPORT_MULTI_H
