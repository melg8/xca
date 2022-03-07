#ifndef HELP_H
#define HELP_H

#include "ui_help.h"

#include <QDialog>

class QHelpEngineCore;
class QHelpLink;

class Help final : public QWidget, public Ui::Help {
  Q_OBJECT

  QHelpEngineCore* helpengine;
  void display(const QUrl& url);

 public:
  Help();
  ~Help() final;
  void register_ctxhelp_button(QDialog* dlg, const QString& help_ctx) const;
  QList<QHelpLink> url_by_ctx(const QString& ctx) const;

 public slots:
  void contexthelp();
  void contexthelp(const QString& context);
  void content();
};

#endif  // HELP_H
