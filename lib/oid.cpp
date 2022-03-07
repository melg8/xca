#include "oid.h"

/* here we have the possibility to add our own OIDS */

#include "func.h"
#include "xca_warning_core.h"

#include <openssl/objects.h>

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QStringList>

int first_additional_oid = 0;

NIDlist extkeyuse_nid;
NIDlist distname_nid;

QMap<QString, const char*> oid_name_clash;
QMap<QString, int> oid_lower_map;

static void addToLowerMap(int nid) {
  QString n = OBJ_nid2sn(nid);
  QString l = n.toLower();

  if (n != l) {
    oid_lower_map[l] = nid;
  }
  n = OBJ_nid2ln(nid);
  l = n.toLower();
  if (n != l) {
    oid_lower_map[l] = nid;
  }
}

/* reads additional OIDs from a file: oid, sn, ln */
static void insert_new_oid(const QStringList& sl, QString fname, int line) {
  bool differs = false;
  QByteArray in_use, oid, sn, ln;

  if (sl.count() != 3) {
    XCA_WARN(QObject::tr("Error reading config file %1 at line %2")
                 .arg(fname)
                 .arg(line));
    return;
  }
  oid = sl[0].toLatin1();
  sn = sl[1].toLatin1();
  ln = sl[2].toLatin1();

  int nid = OBJ_txt2nid(oid.constData());
  if (nid != NID_undef) {
    /* OID already known by OpenSSL */
    if (sn != OBJ_nid2sn(nid)) {
      /* ... but with a different ShortName */
      qWarning() << "OID: " << oid << "SN differs: " << sn << " "
                 << OBJ_nid2sn(nid);
      oid_name_clash[sn] = OBJ_nid2sn(nid);
      differs = true;
    }
    if (ln != OBJ_nid2ln(nid)) {
      /* ... but with a different LongName */
      qWarning() << "OID: " << oid << "LN differs: " << ln << " "
                 << OBJ_nid2ln(nid);
      oid_name_clash[ln] = OBJ_nid2ln(nid);
      differs = true;
    }
  } else {
    /* Check whether ShortName or LongName are already in use
     * for a different OID */
    if (OBJ_txt2nid(sn.constData()) != NID_undef) {
      in_use = sn;
    }
    if (OBJ_txt2nid(ln.constData()) != NID_undef) {
      in_use = ln;
    }
  }
  ign_openssl_error();
  if (differs) {
    /* OID exists with different SN or LN. The differing names
     * are added as "alias" in "oid_name_clash" used when loading
     * dn.txt and eku.txt */
    XCA_WARN(QObject::tr("The Object '%1' from file %2 line %3 is already "
                         "known as '%4:%5:%6' and should be removed.")
                 .arg(sl.join(":"))
                 .arg(fname)
                 .arg(line)
                 .arg(OBJ_obj2QString(OBJ_nid2obj(nid), 1))
                 .arg(OBJ_nid2sn(nid))
                 .arg(OBJ_nid2ln(nid)));
  } else if (!in_use.isEmpty()) {
    /* OID does not exist, but SN or LN however do.
     * Do NOT create OID and tell the user about */
    nid = OBJ_txt2nid(in_use.constData());
    XCA_WARN(QObject::tr("The identifier '%1' for OID %2 from file %3 line %4 "
                         "is already used for a different OID as '%5:%6:%7' "
                         "and should be changed to avoid conflicts.")
                 .arg(in_use.constData())
                 .arg(oid.constData())
                 .arg(fname)
                 .arg(line)
                 .arg(OBJ_obj2QString(OBJ_nid2obj(nid), 1))
                 .arg(OBJ_nid2sn(nid))
                 .arg(OBJ_nid2ln(nid)));
  } else if (nid == NID_undef) {
    nid = OBJ_create(oid.constData(), sn.constData(), ln.constData());
    qDebug() << "Creating OID:" << fname << line << nid << oid << sn << ln;
    try {
      openssl_error();
      addToLowerMap(nid);
    } catch (errorEx& e) {
      errorEx err(errorEx(
          e.getString() +
          QString("%1:%2 OID: %3").arg(fname).arg(line).arg(oid.constData())));
      XCA_ERROR(err);
    }
  }
}

static void readOIDs(const QString& fname) {
  int line = 0;
  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }
  qDebug() << "Read additional OIDs from" << fname;
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString entry = in.readLine().trimmed();
    line++;
    if (entry.startsWith('#') || entry.isEmpty()) {
      continue;
    }
    insert_new_oid(entry.split(QRegExp("\\s*:\\s*")), fname, line);
  }
}

/* reads a list of OIDs/SNs from a file and turns them into a QValueList
 * of integers, representing the NIDs. Usually to be used by NewX509 for
 * the list of ExtendedKeyUsage and Distinguished Name
 */
static NIDlist readNIDlist(const QString& fname) {
  int line = 0, nid;
  NIDlist nl;
  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly)) {
    return nl;
  }

  QTextStream in(&file);
  while (!in.atEnd()) {
    const char* userdefined;
    QString entry = in.readLine().trimmed();
    line++;
    if (entry.startsWith('#') || entry.isEmpty()) {
      continue;
    }
    userdefined = oid_name_clash[entry];
    if (userdefined) {
      entry = userdefined;
    }
    nid = OBJ_txt2nid(CCHAR(entry));
    if (nid == NID_undef) {
      XCA_WARN(QObject::tr("Unknown object '%1' in file %2 line %3")
                   .arg(entry)
                   .arg(fname)
                   .arg(line));
    } else {
      nl += nid;
    }
  }
  openssl_error();
  return nl;
}

/* creates a new nid list from the given filename */
static NIDlist read_nidlist(const QString& name) {
  NIDlist nl;

  foreach (QString d,
           QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)) {
    nl = readNIDlist(d + "/" + name);
    qDebug() << "Read" << nl.count() << "NIDs from" << d + "/" + name;
    if (nl.count() > 0) {
      break;
    }
  }
  return nl;
}

void initOIDs() {
  first_additional_oid = OBJ_new_nid(0);
  openssl_error();
  for (int i = 0; i < first_additional_oid; i++) {
    addToLowerMap(i);
  }
  ign_openssl_error();

  foreach (QString d,
           QStandardPaths::standardLocations(QStandardPaths::AppDataLocation))
    readOIDs(d + "/oids.txt");

  extkeyuse_nid = read_nidlist("eku.txt");
  distname_nid = read_nidlist("dn.txt");

  openssl_error();
}
