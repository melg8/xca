#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMenu>
#include "ui_main_window.h"

#include "lib/database_model.h"
#include "lib/db_history.h"
#include "lib/dh_gen.h"
#include "lib/main.h"
#include "lib/oid.h"
#include "lib/passwd.h"
#include "lib/xca_progress.h"

#include <QHelpEvent>
#include <QList>
#include <QToolTip>

class db_x509;
class pki_multi;
class NewX509;
class OidResolver;
class QProgressBar;
class DHgen;
class Help;

class tipMenu final : public QMenu {
  Q_OBJECT

 public:
  tipMenu(QString n, QWidget* w) : QMenu(n, w) {}
  bool event(QEvent* e) final {
    if (e->type() == QEvent::ToolTip && activeAction() &&
        activeAction()->toolTip() != activeAction()->text()) {
      const QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
      QToolTip::showText(helpEvent->globalPos(), activeAction()->toolTip());
    } else {
      QToolTip::hideText();
    }
    return QMenu::event(e);
  }
};

class MainWindow final : public QMainWindow, public Ui::MainWindow {
  Q_OBJECT

 private:
  static OidResolver* resolver;
  QString string_opt;
  QList<QWidget*> wdList;
  QList<QWidget*> wdMenuList;
  QList<QWidget*> scardList;
  QList<QAction*> acList;
  tipMenu* historyMenu;
  void set_geometry(QString geo);
  QLineEdit* searchEdit;
  QStringList urlsToOpen;
  int checkOldGetNewPass(Passwd& pass);
  void checkDB();
  XcaProgress* dhgenProgress;
  DHgen* dhgen;
  [[nodiscard]] static const QList<QStringList> getTranslators();
  QList<XcaTreeView*> views;
  dbhistory history;
  static void exportIndex(const QString& fname, bool hierarchy);

 protected:
  void init_images();
  void init_menu();
  int force_load;
  NIDlist* read_nidlist(QString name);
  QLabel* statusLabel;
  QString homedir;
  void keyPressEvent(QKeyEvent* e) final;
  void update_history_menu();

 public:
  int exitApp;
  QLabel* dbindex;
  Help* helpdlg;
  MainWindow();
  ~MainWindow() final;
  void loadSettings();
  void saveSettings();
  void load_engine();
  static OidResolver* getResolver() { return resolver; }
  bool mkDir(QString dir);
  void setItemEnabled(bool enable);
  void enableTokenMenu(bool enable);
  void importAnything(QString file);
  void importAnything(const QStringList& files);
  void importMulti(pki_multi* multi, int force);
  void dropEvent(QDropEvent* event) final;
  void dragEnterEvent(QDragEnterEvent* event) final;
  static void initResolver();

 public slots:
  enum open_result init_database(const QString& dbName,
                                 const Passwd& pass = Passwd());
  enum open_result setup_open_database();
  void new_database();
  void load_database();
  void close_database();
  static void dump_database();
  static void default_database();
  void about();
  void loadPem();
  bool pastePem(QString text, bool silent = false);
  void pastePem();
  void changeDbPass();
  void openURLs(QStringList& files);
  void openURLs();
  void changeEvent(QEvent* event) final;
  void exportIndex();
  void exportIndexHierarchy();
  void openRemoteSqlDB();
  void generateDHparamDone();

 protected slots:
  void closeEvent(QCloseEvent* event) final;

 private slots:
  void setOptions();
  void manageToken();
  void initToken();
  void changePin(bool so = false);
  void changeSoPin();
  void initPin();
  void generateDHparam();
  void open_database(QAction* a);
};

#endif  // MAIN_WINDOW_H
