#ifndef __MAIN_H
#define __MAIN_H

#include <QString>

class pki_multi;

class MainWindow;
extern MainWindow* mainwin;

extern char segv_data[1024];
// extern bool exitApp;

pki_multi* probeAnything(const QString&, int* ret = nullptr);
int exportIndex(const QString& fname, bool hierarchy);

#endif
