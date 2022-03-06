#include "dh_gen.h"

#include "bio_byte_array.h"
#include "entropy.h"
#include "func.h"
#include "x_file.h"

#include <openssl/dh.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

void DHgen::run() {
  DH* dh = nullptr;
  BioByteArray b;

  try {
    dh = DH_new();
    Q_CHECK_PTR(dh);
    DH_generate_parameters_ex(dh, bits, 2, nullptr);
    openssl_error();
    PEM_write_bio_DHparams(b, dh);
    openssl_error();
  } catch (errorEx& e) {
    err = e;
  }
  XFile file(fname);
  file.open_write();
  file.write(b);

  if (dh) DH_free(dh);
}
