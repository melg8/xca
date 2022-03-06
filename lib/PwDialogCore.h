#ifndef PW_DIALOG_CORE_H
#define PW_DIALOG_CORE_H

#include <QByteArray>
#include "Passwd.h"
#include "pass_info.h"

class PwDialogUI_i {
 public:
  virtual enum open_result execute(pass_info* p,
                                   Passwd* passwd,
                                   bool write = false,
                                   bool abort = false) = 0;
  virtual ~PwDialogUI_i() {}
};

class PwDialogCore {
 private:
  static PwDialogUI_i* pwdialog;

 public:
  static enum open_result execute(pass_info* p,
                                  Passwd* passwd,
                                  bool write = false,
                                  bool abort = false);
  static int pwCallback(char* buf, int size, int rwflag, void* userdata);
  static void setGui(PwDialogUI_i* p);
};

#endif  // PW_DIALOG_CORE_H
