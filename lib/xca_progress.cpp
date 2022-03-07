#include "xca_progress.h"

#include <cstdio>
#include <cstring>

#include "base.h"

void XcaProgressCmd::stop() { puts(" finished."); }

void XcaProgressCmd::start(const QString& what, int) {
  puts("");
  puts(CCHAR(what));
}

void XcaProgressCmd::increment() {
  static const char* spinner = "|/-\\";
  printf("\rGenerating %c ...", spinner[i++ % 4]);
  fflush(stdout);
}

XcaProgress_i* XcaProgress::progress;

XcaProgress::XcaProgress(const QString& what, int max) {
  if (!progress) progress = new XcaProgressCmd();
  progress->start(what, max);
}

XcaProgress::~XcaProgress() { progress->stop(); }

void XcaProgress::increment() { progress->increment(); }

void XcaProgress::inc(int, int, void* p) {
  auto* prog = static_cast<XcaProgress*>(p);
  if (prog) prog->increment();
}

void XcaProgress::setGui(XcaProgress_i* p) {
  delete progress;
  progress = p;
}
