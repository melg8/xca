#ifndef SEARCH_PKCS_11_DIALOG_H
#define SEARCH_PKCS_11_DIALOG_H

#include <QThread>

#include "ui_search_pkcs_11.h"

class SearchPkcs11;
class searchThread final : public QThread {
  Q_OBJECT

 protected:
  QString dirname;
  QStringList ext;
  bool recursive;
  bool keepOnRunning;

  bool checkLib(QString file);

 public:
  searchThread(QString _dir, const QStringList _ext, bool _recursive);
  void search(QString mydir);
  void run() final { search(dirname); }

 public slots:
  void cancelSearch();

 signals:
  void updateCurrFile(QString f);
  void updateLibs(QString f);
};

class SearchPkcs11 final : public QDialog, public Ui::SearchPkcs11 {
  Q_OBJECT

 protected:
  void searchDir(QString dirname, bool subdirs);
  searchThread* searching;

 public:
  SearchPkcs11(QWidget* parent, const QString& fname);
  ~SearchPkcs11() final;

 public slots:
  void on_search_clicked();
  void on_fileBut_clicked();
  void buttonPress(QAbstractButton* but);
  void loadItem(QListWidgetItem* lib);
  void updateLibs(QString f);
  void updateCurrFile(QString f);
  void finishSearch();

 signals:
  void addLib(QString);
};

#endif  // SEARCH_PKCS_11_DIALOG_H