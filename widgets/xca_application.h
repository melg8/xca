#ifndef XCA_APPLICATION_H
#define XCA_APPLICATION_H

#include <QApplication>
#include <QLocale>
#include <QTranslator>

class MainWindow;
class QAction;

class XcaTranslator : public QTranslator {
  Q_OBJECT
 public:
  XcaTranslator(QObject* p = nullptr) : QTranslator(p) {}
  bool load(const QLocale& locale,
            const QString& filename,
            const QString& dir) {
    return QTranslator::load(QString("%1_%2").arg(filename).arg(locale.name()),
                             dir);
  }
};

class XcaApplication final : public QApplication {
  Q_OBJECT

 private:
  MainWindow* mainw;
  XcaTranslator* qtTr;
  XcaTranslator* xcaTr;
  static QList<QLocale> langAvail;

 public:
  XcaApplication(int& argc, char* argv[]);
  ~XcaApplication() final;
  void setMainwin(MainWindow* m);
  void setupLanguage(const QLocale& lang);
  static QFont tableFont;
  static bool languageAvailable(const QLocale& l);
  bool eventFilter(QObject* watched, QEvent* ev) final;
  bool notify(QObject* receiver, QEvent* event) final;

 public slots:
  void switchLanguage(QAction* a);
  void quit();
};

#endif  // XCA_APPLICATION_H