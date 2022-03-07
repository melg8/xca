#include "pw_dialog_core.h"

#include "xca_warning_core.h"

#include "lib/base.h"
#include "lib/exception.h"
#include "lib/func.h"
#include "lib/passwd.h"

#include <QLabel>
#include <QMessageBox>

PwDialogUI_i* PwDialogCore::pwdialog;

enum open_result PwDialogCore::execute(pass_info* p,
                                       Passwd* passwd,
                                       bool,
                                       bool) {
  if (pwdialog) {
    return pwdialog->execute(p, passwd);
  }
#if !defined(Q_OS_WIN32)
  console_write(stdout, QString(COL_CYAN "%1\n" COL_LRED "%2:" COL_RESET)
                            .arg(p->getDescription())
                            .arg(QObject::tr("Password"))
                            .toUtf8());
  *passwd = readPass();
  return pw_ok;
#else
  throw pw_exit;
#endif
}

int PwDialogCore::pwCallback(char* buf, int size, int rwflag, void* userdata) {
  Passwd passwd;
  enum open_result result;
  auto* p = static_cast<pass_info*>(userdata);

  result = execute(p, &passwd, rwflag, false);

  size = MIN(size, passwd.size());
  memcpy(buf, passwd.constData(), size);
  p->setResult(result);
  return result == pw_ok ? size : 0;
}

void PwDialogCore::setGui(PwDialogUI_i* p) {
  delete pwdialog;
  pwdialog = p;
}
