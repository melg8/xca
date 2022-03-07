#include "pki_evp.h"

#include "bio_byte_array.h"
#include "entropy.h"
#include "func.h"
#include "openssl_compat.h"
#include "pass_info.h"
#include "xca_progress.h"

#include "pw_dialog_core.h"
#include "xca_warning_core.h"

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/rand.h>

Passwd pki_evp::passwd;

QString pki_evp::passHash = QString();

void pki_evp::init() {
  ownPass = ptCommon;
  pkiType = asym_key;
}

void pki_evp::setOwnPass(enum passType x) {
  EVP_PKEY *pk = nullptr, *pk_back = key;
  enum passType oldOwnPass = ownPass;

  if (ownPass == x || isPubKey()) return;

  try {
    pk = decryptKey();
    if (pk == nullptr) return;

    key = pk;
    ownPass = x;
    encryptKey();
  } catch (errorEx& err) {
    if (pk) EVP_PKEY_free(pk);
    key = pk_back;
    ownPass = oldOwnPass;
    throw(err);
  }
}

bool pki_evp::sqlUpdatePrivateKey() {
  Transaction;
  if (!TransBegin()) return false;
  XSqlQuery q;
  SQL_PREPARE(q,
              "UPDATE private_keys SET private=?, ownPass=? "
              "WHERE item=?");
  q.bindValue(0, encKey_b64());
  q.bindValue(1, ownPass);
  q.bindValue(2, sqlItemId);
  AffectedItems(sqlItemId);
  q.exec();

  encKey.fill(0);
  encKey.clear();

  if (!q.lastError().isValid() && q.numRowsAffected() == 1) {
    TransCommit();
    return true;
  }
  return false;
}

void pki_evp::generate(const keyjob& task) {
  Entropy::seed_rng();
  XcaProgress progress;

  BN_GENCB* bar = BN_GENCB_new();
  BN_GENCB_set_old(bar, XcaProgress::inc, &progress);

  switch (task.ktype.type) {
    case EVP_PKEY_RSA: {
      RSA* rsakey = RSA_new();
      BIGNUM* e = BN_new();
      BN_set_word(e, 0x10001);
      if (RSA_generate_key_ex(rsakey, task.size, e, bar))
        EVP_PKEY_assign_RSA(key, rsakey);
      else
        RSA_free(rsakey);
      BN_free(e);
      break;
    }
    case EVP_PKEY_DSA: {
      DSA* dsakey = DSA_new();
      if (DSA_generate_parameters_ex(dsakey, task.size, nullptr, 0, nullptr,
                                     nullptr, bar) &&
          DSA_generate_key(dsakey))
        EVP_PKEY_assign_DSA(key, dsakey);
      else
        DSA_free(dsakey);
      break;
    }
#ifndef OPENSSL_NO_EC
    case EVP_PKEY_EC: {
      EC_KEY* eckey;
      EC_GROUP* group = EC_GROUP_new_by_curve_name(task.ec_nid);
      if (!group) break;
      eckey = EC_KEY_new();
      if (eckey == nullptr) {
        EC_GROUP_free(group);
        break;
      }
      EC_GROUP_set_asn1_flag(group, 1);
      if (EC_KEY_set_group(eckey, group)) {
        if (EC_KEY_generate_key(eckey)) {
          EVP_PKEY_assign_EC_KEY(key, eckey);
          EC_GROUP_free(group);
          break;
        }
      }
      EC_KEY_free(eckey);
      EC_GROUP_free(group);
      break;
    }
#ifdef EVP_PKEY_ED25519
    case EVP_PKEY_ED25519: {
      EVP_PKEY* pkey = nullptr;
      EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
      EVP_PKEY_keygen_init(pctx);
      EVP_PKEY_keygen(pctx, &pkey);
      EVP_PKEY_CTX_free(pctx);
      EVP_PKEY_free(key);
      key = pkey;
    }
#endif
#endif
  }
  BN_GENCB_free(bar);
  isPub = false;
  pkiSource = generated;
  pki_openssl_error();
  encryptKey();
}

pki_evp::pki_evp(const pki_evp* pk) : pki_key(pk) {
  init();
  pki_openssl_error();
  ownPass = pk->ownPass;
  isPub = pk->isPub;
  encKey = pk->getEncKey();
}

pki_evp::pki_evp(const QString& n, int type) : pki_key(n) {
  init();
  EVP_PKEY_set_type(key, type);
  pki_openssl_error();
}

static bool EVP_PKEY_isPrivKey(EVP_PKEY* key) {
  const BIGNUM* b;
  int keytype = EVP_PKEY_id(key);

  switch (EVP_PKEY_type(keytype)) {
    case EVP_PKEY_RSA:
      RSA_get0_key(EVP_PKEY_get0_RSA(key), nullptr, nullptr, &b);
      return b ? true : false;
    case EVP_PKEY_DSA:
      DSA_get0_key(EVP_PKEY_get0_DSA(key), nullptr, &b);
      return b ? true : false;
#ifndef OPENSSL_NO_EC
    case EVP_PKEY_EC:
      return EC_KEY_get0_private_key(EVP_PKEY_get0_EC_KEY(key)) ? true : false;
#ifdef EVP_PKEY_ED25519
    case EVP_PKEY_ED25519: {
      unsigned char buf[ED25519_KEYLEN];
      size_t len = sizeof buf;
      int ret = EVP_PKEY_get_raw_private_key(key, buf, &len);
      ign_openssl_error();
      return ret && len == ED25519_KEYLEN;
    }
#endif
#endif
  }
  return false;
}

pki_evp::pki_evp(EVP_PKEY* pkey) : pki_key() {
  init();
  set_EVP_PKEY(pkey);
}

bool pki_evp::openssl_pw_error() const {
  switch (ERR_peek_error() & 0xff000fff) {
    case ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_DECRYPT):
    case ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_PASSWORD_READ):
    case ERR_PACK(ERR_LIB_EVP, 0, EVP_R_BAD_DECRYPT):
      pki_ign_openssl_error();
      return true;
  }
  return false;
}

void pki_evp::fromPEMbyteArray(const QByteArray& ba, const QString& name) {
  EVP_PKEY* pkey;
  pass_info p(
      XCA_TITLE,
      tr("Please enter the password to decrypt the private key %1.").arg(name));
  pkey = load_ssh_ed25519_privatekey(ba, p);

  while (!pkey) {
    pkey = PEM_read_bio_PrivateKey(BioByteArray(ba).ro(), nullptr,
                                   PwDialogCore::pwCallback, &p);
    if (openssl_pw_error()) XCA_PASSWD_ERROR();
    if (p.getResult() != pw_ok) throw p.getResult();
    if (pki_ign_openssl_error()) break;
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey =
        PEM_read_bio_PUBKEY(BioByteArray(ba).ro(), nullptr, nullptr, nullptr);
  }
  pki_openssl_error();
  set_EVP_PKEY(pkey, name);
}

static void search_ec_oid(EVP_PKEY* pkey) {
#ifndef OPENSSL_NO_EC
  EC_GROUP* builtin;
  const EC_KEY* ec;
  const EC_GROUP* ec_group;

  int keytype = EVP_PKEY_id(pkey);

  if (keytype != EVP_PKEY_EC) return;

  ec = EVP_PKEY_get0_EC_KEY(pkey);
  if (!ec) return;

  ec_group = EC_KEY_get0_group(ec);
  if (!ec_group) return;
  if (EC_GROUP_get_curve_name(ec_group)) return;
  /* There is an EC_GROUP with a missing OID
   * because of explicit parameters */
  foreach (builtin_curve curve, builtinCurves) {
    builtin = EC_GROUP_new_by_curve_name(curve.nid);
    if (EC_GROUP_cmp(builtin, ec_group, nullptr) == 0) {
      EC_GROUP_set_curve_name(const_cast<EC_GROUP*>(ec_group), curve.nid);
      EC_GROUP_set_asn1_flag(const_cast<EC_GROUP*>(ec_group), 1);
      EC_GROUP_free(builtin);
      break;
    }
    EC_GROUP_free(builtin);
  }
#else
  (void)pkey;
#endif
}

void pki_evp::set_EVP_PKEY(EVP_PKEY* pkey, QString name) {
  if (!pkey) return;
  if (!verify(pkey)) {
    pki_ign_openssl_error();
    EVP_PKEY_free(pkey);
    throw errorEx(
        tr("The key from file '%1' is incomplete or inconsistent.").arg(name));
  }
  if (key) EVP_PKEY_free(key);
  key = pkey;
  isPub = !EVP_PKEY_isPrivKey(key);
  if (!isPub) bogusEncryptKey();
  search_ec_oid(pkey);

  autoIntName(name);
  setFilename(name);
  pki_openssl_error();
}

EVP_PKEY* pki_evp::load_ssh_ed25519_privatekey(const QByteArray& ba,
                                               const pass_info& p) {
  EVP_PKEY* pkey = nullptr;
  unsigned char* pdata;
  long plen;
  QByteArray chunk, enc_algo, kdfname, kdf, pub, priv;

  (void)p;  // Will be used later for decryption
  if (!PEM_bytes_read_bio(&pdata, &plen, nullptr, PEM_STRING_OPENSSH_KEY,
                          BioByteArray(ba).ro(), nullptr, nullptr)) {
    return nullptr;
  }

  QByteArray content((const char*)pdata, plen);
  OPENSSL_free(pdata);

  if (!content.startsWith("openssh-key-v1") ||
      // also check trailing \0
      content.constData()[sizeof "openssh-key-v1" - 1])
    return nullptr;

  content.remove(0, sizeof "openssh-key-v1");
  // encryption: "none", "aes256-ctr"
  enc_algo = ssh_key_next_chunk(&content);
  // KDFName "bcrypt"
  kdfname = ssh_key_next_chunk(&content);
  kdf = ssh_key_next_chunk(&content);

  if (enc_algo != "none" || kdfname != "none") {
    qCritical("Encrypted SSH ED25519 keys not supported, yet");
    return nullptr;
  }
  // check bytes 00 00 00 01
  const char* d = content.constData();
  if (d[0] || d[1] || d[2] || d[3] != 1) return nullptr;
  content.remove(0, 4);
  // Handle first occurance of the public key
  pub = ssh_key_next_chunk(&content);
  ssh_key_check_chunk(&pub, "ssh-ed25519");
  pub = ssh_key_next_chunk(&pub);
  if (pub.count() != ED25519_KEYLEN) return nullptr;

  // Followed by the private key
  priv = ssh_key_next_chunk(&content);
  // Drop 64bit random nonce
  priv.remove(0, 8);

  ssh_key_check_chunk(&priv, "ssh-ed25519");
  // The first pubkey must match the second occurance
  // in front of the private one
  if (pub != ssh_key_next_chunk(&priv)) return nullptr;
  priv = ssh_key_next_chunk(&priv);
  // The private key is concatenated by the public key in one chunk
  if (priv.count() != 2 * ED25519_KEYLEN) return nullptr;
  // The last ED25519_KEYLEN bytes must match the public key
  if (pub != priv.mid(ED25519_KEYLEN)) return nullptr;
    // The first ED25519_KEYLEN octets are the private key
#ifndef OPENSSL_NO_EC
#ifdef EVP_PKEY_ED25519
  pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr,
                                      (const unsigned char*)priv.constData(),
                                      ED25519_KEYLEN);
#endif
#endif
  pki_openssl_error();
  return pkey;
}

void pki_evp::fload(const QString& fname) {
  pass_info p(
      XCA_TITLE,
      tr("Please enter the password to decrypt the private key from file:\n%1")
          .arg(compressFilename(fname)));
  pem_password_cb* cb = PwDialogCore::pwCallback;

  pki_ign_openssl_error();
  XFile file(fname);
  file.open_read();
  QByteArray ba = file.readAll();
  EVP_PKEY* pkey;

  do {
    pkey = PEM_read_bio_PrivateKey(BioByteArray(ba).ro(), nullptr, cb, &p);
    if (openssl_pw_error()) XCA_PASSWD_ERROR();
    if (p.getResult() != pw_ok) throw p.getResult();
    if (pki_ign_openssl_error()) break;
  } while (!pkey);

  if (!pkey) {
    pki_ign_openssl_error();
    pkey = d2i_PrivateKey_bio(BioByteArray(ba).ro(), nullptr);
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey = d2i_PKCS8PrivateKey_bio(BioByteArray(ba).ro(), nullptr, cb, &p);
  }
  if (!pkey) {
    PKCS8_PRIV_KEY_INFO* p8inf;
    pki_ign_openssl_error();
    p8inf = d2i_PKCS8_PRIV_KEY_INFO_bio(BioByteArray(ba).ro(), nullptr);
    if (p8inf) {
      pkey = EVP_PKCS82PKEY(p8inf);
      PKCS8_PRIV_KEY_INFO_free(p8inf);
    }
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey = b2i_PVK_bio(BioByteArray(ba).ro(), cb, &p);
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey = load_ssh_ed25519_privatekey(ba, p);
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey = PEM_read_bio_PUBKEY(BioByteArray(ba).ro(), nullptr, cb, &p);
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey = d2i_PUBKEY_bio(BioByteArray(ba).ro(), nullptr);
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey = load_ssh2_key(ba);
  }
  if (!pkey) {
    pki_ign_openssl_error();
    pkey = b2i_PublicKey_bio(BioByteArray(ba).ro());
  }
  if (pki_ign_openssl_error() || !pkey) {
    if (pkey) EVP_PKEY_free(pkey);
    throw errorEx(tr("Unable to load the private key in file %1. Tried PEM and "
                     "DER private, public, PKCS#8 key types and SSH2 format.")
                      .arg(fname));
  }
  set_EVP_PKEY(pkey, fname);
}

EVP_PKEY* pki_evp::decryptKey() const {
  Passwd ownPassBuf;
  int ret;

  if (isPubKey()) {
    QByteArray ba = i2d_bytearray(I2D_VOID(i2d_PUBKEY), key);
    return (EVP_PKEY*)d2i_bytearray(D2I_VOID(d2i_PUBKEY), ba);
  }
  /* This key has its own password */
  if (ownPass == ptPrivate) {
    pass_info pi(
        XCA_TITLE,
        tr("Please enter the password to decrypt the private key: '%1'")
            .arg(getIntName()));
    ret = PwDialogCore::execute(&pi, &ownPassBuf, false);
    if (ret != 1) throw errorEx(tr("Password input aborted"), getClassName());
  } else if (ownPass == ptBogus) {  // BOGUS pass
    ownPassBuf = "Bogus";
  } else {
    ownPassBuf = passwd;
    while (passHash.isEmpty() ||
           (sha512passwT(ownPassBuf, passHash) != passHash &&
            sha512passwd(ownPassBuf, passHash) != passHash)) {
      pass_info p(
          XCA_TITLE,
          tr("Please enter the database password for decrypting the key '%1'")
              .arg(getIntName()));
      ret = PwDialogCore::execute(&p, &ownPassBuf, passHash.isEmpty());
      if (ret != 1) throw errorEx(tr("Password input aborted"), getClassName());
    }
  }
  QByteArray myencKey = getEncKey();
  qDebug() << "myencKey.count()" << myencKey.count();
  if (myencKey.count() == 0) return nullptr;
  EVP_PKEY* priv = nullptr;
  X509_SIG* p8 = d2i_PKCS8_bio(BioByteArray(myencKey).ro(), nullptr);
  if (p8) {
    PKCS8_PRIV_KEY_INFO* p8inf =
        PKCS8_decrypt(p8, ownPassBuf.constData(), ownPassBuf.size());
    if (p8inf) {
      priv = EVP_PKCS82PKEY(p8inf);
      PKCS8_PRIV_KEY_INFO_free(p8inf);
    }
    X509_SIG_free(p8);
  }
  pki_ign_openssl_error();
  return priv;
}

EVP_PKEY* pki_evp::priv2pub(EVP_PKEY* privateKey) {
  int keylen;
  unsigned char *p, *p1;
  EVP_PKEY* pubkey;

  keylen = i2d_PUBKEY(privateKey, nullptr);
  p1 = p = (unsigned char*)OPENSSL_malloc(keylen);
  Q_CHECK_PTR(p);

  /* convert rsa/dsa/ec to Pubkey */
  keylen = i2d_PUBKEY(privateKey, &p);
  pki_openssl_error();
  p = p1;
  pubkey = d2i_PUBKEY(nullptr, const_cast<const unsigned char**>(&p), keylen);
  OPENSSL_free(p1);
  pki_openssl_error();
  return pubkey;
}

void pki_evp::encryptKey(const char* password) {
  Passwd ownPassBuf;

  pki_openssl_error();
  /* This key has its own, private password */
  if (ownPass == ptPrivate) {
    int ret;
    pass_info p(XCA_TITLE,
                tr("Please enter the password to protect the private key: '%1'")
                    .arg(getIntName()));
    ret = PwDialogCore::execute(&p, &ownPassBuf, true);
    if (ret != 1) throw errorEx("Password input aborted", getClassName());
    pki_openssl_error();
  } else if (ownPass == ptBogus) {  // BOGUS password
    ownPassBuf = "Bogus";
    pki_openssl_error();
  } else {
    if (password) {
      /* use the password parameter
       * if this is a common password */
      ownPassBuf = password;
      pki_openssl_error();
    } else {
      int ret = 0;
      ownPassBuf = passwd;
      pass_info p(
          XCA_TITLE,
          tr("Please enter the database password for encrypting the key"));
      while (passHash.isEmpty() ||
             (sha512passwT(ownPassBuf, passHash) != passHash &&
              sha512passwd(ownPassBuf, passHash) != passHash)) {
        ret = PwDialogCore::execute(&p, &ownPassBuf, passHash.isEmpty());
        if (ret != 1) throw errorEx("Password input aborted", getClassName());
      }
    }
  }

  /* Convert private key to DER(PKCS8-aes) */
  BioByteArray bba;
  i2d_PKCS8PrivateKey_bio(bba, key, EVP_aes_256_cbc(), ownPassBuf.data(),
                          ownPassBuf.size(), nullptr, nullptr);
  pki_openssl_error();
  encKey = bba;

  /* Replace private key by public key and
     have the encrypted private in "encKey"
   */
  EVP_PKEY* pkey1 = priv2pub(key);
  Q_CHECK_PTR(pkey1);
  EVP_PKEY_free(key);
  key = pkey1;
  pki_openssl_error();
}

void pki_evp::set_evp_key(EVP_PKEY* pkey) {
  if (key) free(key);
  key = pkey;
}

void pki_evp::bogusEncryptKey() {
  ownPass = ptBogus;
  isPub = false;
  encryptKey();
}

pki_evp::~pki_evp() { encKey.fill(0); }

QSqlError pki_evp::insertSqlData() {
  XSqlQuery q;
  QSqlError e = pki_key::insertSqlData();
  if (e.isValid()) return e;
  if (isPubKey()) return {};

  SQL_PREPARE(q,
              "INSERT INTO private_keys (item, ownPass, private) "
              "VALUES (?, ?, ?)");
  q.bindValue(0, sqlItemId);
  q.bindValue(1, ownPass);
  q.bindValue(2, encKey_b64());
  q.exec();
  encKey.fill(0);
  encKey.clear();
  return q.lastError();
}

void pki_evp::restoreSql(const QSqlRecord& rec) {
  pki_key::restoreSql(rec);
  isPub = rec.isNull(VIEW_private_ownpass);
  if (!isPub) {
    const int value = rec.value(VIEW_private_ownpass).toInt();
    if (value < 0 || value > 3) {
      ownPass = passType::ptInvalid;
    } else {
      ownPass = static_cast<enum passType>(value);
    }
  }
}

QByteArray pki_evp::getEncKey() const {
  XSqlQuery q;
  QSqlError e;
  QByteArray ba;

  if (encKey.count() > 0 || !sqlItemId.isValid()) return encKey;

  SQL_PREPARE(q, "SELECT private FROM private_keys WHERE item=?");
  q.bindValue(0, sqlItemId);
  q.exec();
  e = q.lastError();
  if (e.isValid() || !q.first()) return {};
  return QByteArray::fromBase64(q.value(0).toByteArray().trimmed());
}

QSqlError pki_evp::deleteSqlData() {
  XSqlQuery q;
  QSqlError e = pki_key::deleteSqlData();
  if (e.isValid()) return e;
  SQL_PREPARE(q, "DELETE FROM private_keys WHERE item=?");
  q.bindValue(0, sqlItemId);
  q.exec();
  return q.lastError();
}

bool pki_evp::pem(BioByteArray& b) {
  EVP_PKEY* pkey;
  int keytype;
  const pki_export* xport = pki_export::by_id(Settings["KeyFormat"]);

  if (xport->match_all(F_PEM | F_PRIVATE)) {
    pkey = decryptKey();
    keytype = EVP_PKEY_id(pkey);
    switch (keytype) {
      case EVP_PKEY_RSA:
        PEM_write_bio_RSAPrivateKey(b, EVP_PKEY_get0_RSA(pkey), nullptr,
                                    nullptr, 0, nullptr, nullptr);
        break;
      case EVP_PKEY_DSA:
        PEM_write_bio_DSAPrivateKey(b, EVP_PKEY_get0_DSA(pkey), nullptr,
                                    nullptr, 0, nullptr, nullptr);
        break;
#ifndef OPENSSL_NO_EC
      case EVP_PKEY_EC:
        PEM_write_bio_ECPrivateKey(b, EVP_PKEY_get0_EC_KEY(pkey), nullptr,
                                   nullptr, 0, nullptr, nullptr);
        break;
#ifdef EVP_PKEY_ED25519
      case EVP_PKEY_ED25519:
        if (xport->match_all(F_PRIVATE)) return false;
        write_SSH2_ed25519_private(b, pkey, nullptr);
        break;
#endif
#endif
    }
    EVP_PKEY_free(pkey);
  } else if (xport->match_all(F_PKCS8 | F_PRIVATE)) {
    const EVP_CIPHER* algo =
        xport->match_all(F_CRYPT) ? EVP_aes_256_cbc() : nullptr;
    pkey = decryptKey();
    PEM_write_bio_PrivateKey(b, pkey, algo,
                             const_cast<unsigned char*>(passwd.constUchar()),
                             passwd.size(), nullptr, nullptr);
    EVP_PKEY_free(pkey);
  } else
    return pki_key::pem(b);

  return true;
}
void pki_evp::writePKCS8(XFile& file,
                         const EVP_CIPHER* enc,
                         pem_password_cb* cb,
                         bool pem) const {
  pass_info p(XCA_TITLE,
              tr("Please enter the password protecting the PKCS#8 key '%1'")
                  .arg(getIntName()));
  EVP_PKEY* pkey = decryptKey();
  if (!pkey) {
    pki_openssl_error();
    return;
  }
  BioByteArray b;
  if (pem) {
    b += PEM_comment();
    PEM_write_bio_PKCS8PrivateKey(b, pkey, enc, nullptr, 0, cb, &p);
  } else {
    i2d_PKCS8PrivateKey_bio(b, pkey, enc, nullptr, 0, cb, &p);
  }
  EVP_PKEY_free(pkey);
  file.write(b);
}

void pki_evp::writePVKprivate(XFile& file, pem_password_cb* cb) const {
  pass_info p(
      XCA_TITLE,
      tr("Please enter the password protecting the Microsoft PVK key '%1'")
          .arg(getIntName()));

  int enc = cb ? 2 /* pvk-strong */ : 0 /* pvk-none */;
  EVP_PKEY* pkey = decryptKey();
  if (!pkey) {
    pki_openssl_error();
    return;
  }
  /* In case of success! the error
   *   PEMerr(PEM_F_I2B_PVK_BIO, PEM_R_BIO_WRITE_FAILURE)
   * is set. Workaround this behavior */
  BioByteArray b;
  if (i2b_PVK_bio(b, pkey, enc, cb, &p) == -1) {
    pki_openssl_error();
    PEMerr(PEM_F_I2B_PVK_BIO, PEM_R_BIO_WRITE_FAILURE);
    pki_openssl_error();
  }
  ign_openssl_error();
  EVP_PKEY_free(pkey);
  file.write(b);
}

static int mycb(char* buf, int size, int, void*) {
  strncpy(buf, pki_evp::passwd, size);
  return strlen(pki_evp::passwd);
}

void pki_evp::writeDefault(const QString& dirname) const {
  XFile file(get_dump_filename(dirname, ".pem"));
  file.open_key();
  writeKey(file, pki_evp::passwd[0] ? EVP_des_ede3_cbc() : nullptr, mycb, true);
}

#ifndef LIBRESSL_VERSION_NUMBER
int PEM_write_bio_PrivateKey_traditional(BIO* bp,
                                         EVP_PKEY* x,
                                         const EVP_CIPHER* enc,
                                         unsigned char* kstr,
                                         int klen,
                                         pem_password_cb* cb,
                                         void* u) {
  QString pem = keytype::byPKEY(x).traditionalPemName();

  return PEM_ASN1_write_bio((i2d_of_void*)i2d_PrivateKey, pem.toLatin1(), bp,
                            (char*)x, enc, kstr, klen, cb, u);
}
#endif

void pki_evp::writeKey(XFile& file,
                       const EVP_CIPHER* enc,
                       pem_password_cb* cb,
                       bool pem) const {
  pass_info p(XCA_TITLE,
              tr("Please enter the export password for the private key '%1'")
                  .arg(getIntName()));

  if (isPubKey()) {
    writePublic(file, pem);
    return;
  }
  EVP_PKEY* pkey = key ? decryptKey() : nullptr;
  if (!pkey) {
    pki_openssl_error();
    return;
  }
  BioByteArray b;
  if (pem) {
    b += PEM_comment();
    PEM_write_bio_PrivateKey_traditional(b, pkey, enc, nullptr, 0, cb, &p);
  } else {
    i2d_PrivateKey_bio(b, pkey);
  }
  EVP_PKEY_free(pkey);
  pki_openssl_error();
  file.write(b);
}

bool pki_evp::verify_priv(EVP_PKEY* pkey) const {
  bool verify = true;
  unsigned char data[32], sig[1024];
  size_t datalen = sizeof data, siglen = sizeof sig;
  EVP_MD_CTX* ctx = nullptr;
  const EVP_MD* md = EVP_sha256();
  EVP_PKEY_CTX* pkctx = nullptr;

  if (!EVP_PKEY_isPrivKey(pkey)) return true;
  do {
    ctx = EVP_MD_CTX_new();
    pki_ign_openssl_error();
    RAND_bytes(data, datalen);
    Q_CHECK_PTR(ctx);
    verify = false;

    /* Sign some random data in "data" */
#ifdef EVP_PKEY_ED25519
    if (EVP_PKEY_id(pkey) == EVP_PKEY_ED25519) md = nullptr;
#endif
    if (!EVP_DigestSignInit(ctx, &pkctx, md, nullptr, pkey)) break;

    if (EVP_PKEY_id(pkey) == EVP_PKEY_RSA)
      EVP_PKEY_CTX_set_rsa_padding(pkctx, RSA_PKCS1_PADDING);

    if (!EVP_DigestSign(ctx, sig, &siglen, data, datalen)) break;

    /* Verify the signature */
    if (!EVP_DigestVerifyInit(ctx, nullptr, md, nullptr, pkey)) break;

    if (EVP_DigestVerify(ctx, sig, siglen, data, datalen) != 1) break;

    verify = true;
  } while (0);

  if (ctx) EVP_MD_CTX_free(ctx);

  if (EVP_PKEY_id(pkey) == EVP_PKEY_RSA && EVP_PKEY_isPrivKey(pkey)) {
    const RSA* rsa = EVP_PKEY_get0_RSA(pkey);
    if (RSA_check_key(rsa) != 1) verify = false;
  }
  pki_openssl_error();
  return verify;
}

QVariant pki_evp::getIcon(const dbheader* hd) const {
  if (hd->id != HD_internal_name) return {};

  return QVariant(QPixmap(isPubKey() ? ":pubkeyIco" : ":keyIco"));
}

QString pki_evp::md5passwd(QByteArray pass) {
  return formatHash(Digest(pass, EVP_md5()));
}

QString pki_evp::_sha512passwd(QByteArray pass,
                               QString salt,
                               int size,
                               int repeat) {
  if (salt.length() < size) return {};

  salt = salt.left(size);
  pass = salt.toLatin1() + pass;

  while (repeat--) pass = Digest(pass, EVP_sha512());

  return salt + formatHash(pass, "");
}

QString pki_evp::sha512passwd(QByteArray pass, QString salt) {
  return _sha512passwd(pass, salt, 5, 1);
}

QString pki_evp::sha512passwT(QByteArray pass, QString salt) {
  return _sha512passwd(pass, salt, 17, 8000);
}
