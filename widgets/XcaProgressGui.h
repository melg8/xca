#ifndef __XCAPROGRESSGUI_H
#define __XCAPROGRESSGUI_H

#include "lib/XcaProgress.h"

#include "lib/base.h"
#include "lib/main.h"

#include <QMainWindow>
#include <QProgressBar>
#include <QStatusBar>

class XcaProgressGui final : public XcaProgress_i {
 private:
  QMainWindow* mwin;
  QProgressBar* bar;

 public:
  XcaProgressGui() = delete;
  XcaProgressGui(QMainWindow* m) : XcaProgress_i(), mwin(m), bar() {
    bar = new QProgressBar();
    bar->setMinimum(0);
    bar->setTextVisible(true);
  }
  void start(const QString& what, int max) final {
    bar->setValue(0);
    bar->setMaximum(max);
    bar->setFormat(what);
    mwin->statusBar()->addPermanentWidget(bar, 1);
    bar->show();
    mwin->repaint();
  }
  void stop() final { mwin->statusBar()->removeWidget(bar); }
  ~XcaProgressGui() final { delete bar; }
  void increment() final {
    int value = bar->value();
    if (value == bar->maximum()) {
      bar->reset();
    } else {
      bar->setValue(value + 1);
    }
  }
};

#endif
