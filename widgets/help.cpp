#include "help.h"

#include "lib/func.h"

#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHelpEngine>
#include <QHelpLink>

Help::Help() : QWidget(nullptr), helpengine(nullptr) {
  setupUi(this);
  setWindowTitle(XCA_TITLE);
  textbox->setSearchPaths(QStringList(getDocDir()));
  textbox->setOpenExternalLinks(true);
  textbox->clearHistory();
  if (!getDocDir().isEmpty())
    helpengine = new QHelpEngineCore(getDocDir() + "/xca.qhc");
}

Help::~Help() { delete helpengine; }

void Help::display(const QUrl& url) {
  textbox->setSource(QUrl(url.fileName()));
  textbox->scrollToAnchor(url.fragment());
  show();
  raise();
}

void Help::content() {
  display(QUrl("qthelp://org.sphinx.xca/doc/index.html"));
}

QList<QHelpLink> Help::url_by_ctx(const QString& ctx) const {
  if (!helpengine) return {};
  return helpengine->documentsForIdentifier(QString("%1.%1").arg(ctx));
}

void Help::contexthelp(const QString& context) {
  QList<QHelpLink> helpctx = url_by_ctx(context);

  if (!helpctx.empty()) display(helpctx.constBegin()->url);
}

void Help::contexthelp() {
  QObject* o = sender();
  if (!o) return;
  QString ctx = o->property("help_ctx").toString();
  if (ctx.isEmpty()) return;
  contexthelp(ctx);
}

void Help::register_ctxhelp_button(QDialog* dlg,
                                   const QString& help_ctx) const {
  auto* buttonBox = dlg->findChild<QDialogButtonBox*>("buttonBox");

  if (!buttonBox || help_ctx.isEmpty()) return;

  dlg->setWindowModality(Qt::WindowModal);
  buttonBox->addButton(QDialogButtonBox::Help);
  buttonBox->setProperty("help_ctx", QVariant(help_ctx));
  connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(contexthelp()));

  if (helpengine && url_by_ctx(help_ctx).count() == 0) {
    qWarning() << "Unknown help context: " << help_ctx;
    buttonBox->button(QDialogButtonBox::Help)->setEnabled(false);
  }
}
