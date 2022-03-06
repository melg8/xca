#ifndef __LOAD_OBJ_H
#define __LOAD_OBJ_H

#include <QStringList>

class pki_base;

class load_base {
 public:
  QString filter;
  QString caption;
  load_base();
  virtual ~load_base();
  virtual pki_base* loadItem(const QString& s);
  virtual pki_base* newItem();
};

class load_key final : public load_base {
 public:
  load_key();
  pki_base* newItem() final;
};

class load_req final : public load_base {
 public:
  load_req();
  pki_base* newItem() final;
};

class load_cert final : public load_base {
 public:
  load_cert();
  pki_base* newItem() final;
};

class load_pkcs7 final : public load_base {
 public:
  load_pkcs7();
  pki_base* newItem() final;
};

class load_pkcs12 final : public load_base {
 public:
  load_pkcs12();
  pki_base* loadItem(const QString& s) final;
};

class load_temp final : public load_base {
 public:
  load_temp();
  pki_base* newItem() final;
};

class load_crl final : public load_base {
 public:
  load_crl();
  pki_base* newItem() final;
};

class load_db final : public load_base {
 public:
  load_db();
};

class load_pkcs11 final : public load_base {
 public:
  load_pkcs11();
};

class load_pem final : public load_base {
 public:
  load_pem();
  pki_base* newItem() final;
};

#endif
