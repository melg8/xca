#include "db_crl.h"

#include "database_model.h"
#include "db_x_509.h"
#include "exception.h"
#include "main.h"

db_crl::db_crl() : db_x509name("crls") {
  sqlHashTable = "crls";
  pkitype << revocation;
  pkitype_depends << x509;
  updateHeaders();
  loadContainer();
}

dbheaderList db_crl::getHeaders() {
  dbheaderList h = db_x509name::getHeaders();
  h << new dbheader(HD_crl_signer, true, tr("Signer"),
                    tr("Internal name of the signer"))
    << new num_dbheader(HD_crl_revoked, true, tr("No. revoked"),
                        tr("Number of revoked certificates"))
    << new date_dbheader(HD_crl_lastUpdate, false, tr("Last update"))
    << new date_dbheader(HD_crl_nextUpdate, true, tr("Next update"))
    << new num_dbheader(HD_crl_crlnumber, false, tr("CRL number"));
  return h;
}

pki_base* db_crl::newPKI(enum pki_type type) {
  (void)type;
  return new pki_crl();
}

void db_crl::revokeCerts(pki_crl* crl) {
  auto* certs = Database.model<db_x509>();
  x509revList revlist;

  if (!certs || !crl) {
    return;
  }

  pki_x509* signer = crl->getIssuer();
  if (!signer) {
    return;
  }

  revlist = crl->getRevList();
  signer->mergeRevList(revlist);
  foreach (x509rev revok, revlist) {
    pki_x509* crt = signer->getBySerial(revok.getSerial());
    if (crt) {
      crt->setRevoked(revok);
    }
  }
}

void db_crl::removeSigner(pki_base* signer) {
  foreach (pki_crl* crl, Store.getAll<pki_crl>()) {
    if (crl->getIssuer() == signer) {
      crl->setIssuer(nullptr);
    }
  }
}

void db_crl::inToCont(pki_base* pki) {
  auto* crl = dynamic_cast<pki_crl*>(pki);
  unsigned hash = crl->getSubject().hashNum();
  QList<pki_x509*> items;

  items = Store.sqlSELECTpki<pki_x509>(
      "SELECT x509super.item FROM x509super "
      "JOIN certs ON certs.item = x509super.item "
      "WHERE x509super.subj_hash=? AND certs.ca=1",
      QList<QVariant>() << QVariant(hash));
  foreach (pki_x509* x, items) {
    qDebug() << "Possible Crl issuer:" << x->getIntName();
    crl->verify(x);
  }
  db_base::inToCont(pki);
}

pki_base* db_crl::insert(pki_base* item) {
  auto* crl = dynamic_cast<pki_crl*>(item);
  auto* oldcrl = dynamic_cast<pki_crl*>(getByReference(crl));
  if (oldcrl) {
    XCA_INFO(tr("The revocation list already exists in the database "
                "as:\n'%1'\nand so it was not imported")
                 .arg(oldcrl->getIntName()));
    delete (crl);
    return nullptr;
  }
  Transaction;
  if (TransBegin()) {
    crl = dynamic_cast<pki_crl*>(insertPKI(crl));
    if (crl) {
      revokeCerts(crl);
      TransCommit();
    }
  }
  return crl;
}

void db_crl::exportItems(const QModelIndexList& indexes,
                         const pki_export* xport,
                         XFile& file) const {
  QStringList vcal;
  foreach (QModelIndex idx, indexes) {
    auto* crl = fromIndex<pki_crl>(idx);
    if (!crl) {
      continue;
    }
    if (xport->match_all(F_CAL)) {
      vcal << crl->icsVEVENT();
    } else {
      crl->writeCrl(file, xport->match_all(F_PEM));
    }
  }
  if (vcal.size() > 0) {
    writeVcalendar(file, vcal);
  }
}

pki_crl* db_crl::newCrl(const crljob& task) {
  pki_crl* crl = nullptr;
  pki_x509* cert = task.issuer;
  QSqlDatabase db = QSqlDatabase::database();
  try {
    x509v3ext e;
    X509V3_CTX ext_ctx;
    X509V3_set_ctx(&ext_ctx, cert->getCert(), nullptr, nullptr, nullptr, 0);
    X509V3_set_ctx_nodb(&ext_ctx) XSqlQuery q;

    crl = new pki_crl();
    crl->createCrl(cert->getIntName(), cert);
    crl->pkiSource = generated;

    foreach (x509rev rev, cert->getRevList())
      crl->addRev(rev, task.settings.withReason);

    if (task.settings.authKeyId) {
      crl->addV3ext(
          e.create(NID_authority_key_identifier, "keyid,issuer", &ext_ctx));
    }
    if (task.settings.subAltName) {
      if (cert->hasExtension(NID_subject_alt_name)) {
        crl->addV3ext(e.create(NID_issuer_alt_name, "issuer:copy", &ext_ctx));
      }
    }
    if (task.settings.setCrlNumber) {
      crl->setCrlNumber(task.settings.crlNumber);
      cert->setCrlNumber(task.settings.crlNumber);
    }
    crl->setIssuer(cert);
    crl->setLastUpdate(task.settings.lastUpdate);
    crl->setNextUpdate(task.settings.nextUpdate);
    crl->sign(cert->getRefKey(), task.settings.hashAlgo.MD());

    Transaction;
    if (!TransBegin()) {
      throw errorEx(tr("Failed to initiate DB transaction"));
    }
    cert->setCrlExpire(task.settings.nextUpdate);
    SQL_PREPARE(q, "UPDATE authority set crlNo=?, crlExpire=? WHERE item=?");
    q.bindValue(0, (uint)cert->getCrlNumber().getLong());
    q.bindValue(1, task.settings.nextUpdate.toPlain());
    q.bindValue(2, cert->getSqlItemId());
    AffectedItems(cert->getSqlItemId());
    q.exec();
    QSqlError err = q.lastError();
    if (err.isValid()) {
      throw errorEx(tr("Database error: %1").arg(err.text()));
    }
    SQL_PREPARE(q,
                "UPDATE revocations set crlNo=? "
                "WHERE crlNo IS NULL AND caId=?");
    q.bindValue(0, (uint)crl->getCrlNumber().getLong());
    q.bindValue(1, cert->getSqlItemId());
    q.exec();
    err = q.lastError();
    if (err.isValid()) {
      throw errorEx(tr("Database error: %1").arg(err.text()));
    }
    crl = dynamic_cast<pki_crl*>(insertPKI(crl));
    err = db.lastError();
    if (err.isValid()) {
      throw errorEx(tr("Database error: %1").arg(err.text()));
    }
    TransCommit();
    createSuccess(crl);
  } catch (errorEx& err) {
    XCA_ERROR(err);
    delete crl;
    crl = nullptr;
  }
  return crl;
}
