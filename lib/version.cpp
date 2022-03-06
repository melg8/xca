#include "func.h"
#include "local.h"

#define VERSION XCA_VERSION

#ifdef GIT_LOCAL_CHANGES
#define COMMITHASH GIT_COMMIT_REV "+local-changes"
#else
#define COMMITHASH GIT_COMMIT_REV
#endif

// TODO(melg): add proper header with function declaration.
const char* version_str(bool html) {
  return html ? "<b>" VERSION "-dev</b><br/>commit: <b>" COMMITHASH "</b>"
              : VERSION "-dev\ncommit: " COMMITHASH;
}
