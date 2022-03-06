#ifndef __DB_X509_H
#define __DB_X509_H

#include "db_x509super.h"

#include "asn1int.h"
#include "pki_export.h"
#include "x509rev.h"

class pki_x509req;
class pki_x509;
class pki_temp;
class pki_crl;

class db_x509 : public db_x509super {
  Q_OBJECT

 protected:
  pki_x509* get1SelectedCert();
  dbheaderList getHeaders() override;
  void dereferenceIssuer();

 public:
  db_x509();
  pki_base* newPKI(enum pki_type type = none) override;
  pki_x509* findIssuer(pki_x509* client);
  virtual void loadContainer() override;

  bool updateView();
  void updateViewAll();
  void updateViewPKI(pki_base* pki);
  void remFromCont(const QModelIndex& idx) override;
  QList<pki_x509*> getAllIssuers();
  QList<pki_x509*> getCerts(bool unrevoked);
  void writeIndex(const QString& fname, bool hierarchy) const;
  void writeIndex(XFile& file, QList<pki_x509*> items) const;
  pki_base* insert(pki_base* item) override;
  void markRequestSigned(pki_x509req* req, pki_x509* cert);
  pki_x509* newCert(NewX509* dlg);
  void newCert(pki_x509* cert);
  void writePKCS12(pki_x509* cert, XFile& file, bool chain) const;
  void writePKCS7(pki_x509* cert,
                  XFile& file,
                  int flags,
                  const QModelIndexList& list) const;
  void fillContextMenu(QMenu* menu, const QModelIndex& index);
  void inToCont(pki_base* pki) override;
  a1int getUniqueSerial(pki_x509* signer);
  void toToken(QModelIndex idx, bool alwaysSelect);
  void toRequest(QModelIndex idx);
  void updateCaProperties(pki_x509* cert);
  void toCertificate(QModelIndex index);
  void certRenewal(QModelIndexList indexes);
  void revoke(QModelIndexList indexes);
  void do_revoke(QModelIndexList indexes, const x509rev& r);
  void unRevoke(QModelIndexList indexes);
  int exportFlags(const QModelIndex& idx) const override;
  void exportItems(const QModelIndexList& indexes,
                   const pki_export* xport,
                   XFile& file) const override;

 public slots:
  void newItem() override;

  void newCert(pki_temp*);
  void newCert(pki_x509req*);
};

#endif
