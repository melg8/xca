#ifndef PKI_BASE_H
#define PKI_BASE_H

#include <QObject>

#include "asn_1_time.h"
#include "base.h"
#include "bio_byte_array.h"
#include "header_list.h"
#include "pkcs_11_lib.h"
#include "pki_export.h"
#include "pki_lookup.h"
#include "sql.h"
#include "x_file.h"

#include <QByteArray>
#include <QRegExp>
#include <QString>
#include <QVariant>

#define pki_openssl_error() _openssl_error(*this, C_FILE, __LINE__)
#define pki_ign_openssl_error() _ign_openssl_error(*this, C_FILE, __LINE__)

enum pki_source {
  unknown,
  imported,
  generated,
  transformed,
  token,
  legacy_db,
  renewed
};

#define VIEW_item_id 0
#define VIEW_item_name 1
#define VIEW_item_type 2
#define VIEW_item_date 3
#define VIEW_item_source 4
#define VIEW_item_comment 5

extern pki_lookup Store;

class pki_base : public QObject {
  Q_OBJECT

 public: /* static */
  static QRegExp limitPattern;
  static QString rmslashdot(const QString& fname);
  static unsigned hash(const QByteArray& ba);
  static bool pem_comment;
  static int count;
  static QList<pki_base*> allitems;

 protected:
  QVariant sqlItemId;
  QString desc, comment;
  a1time insertion_date;
  enum pki_type pkiType;
  /* model data */
  pki_base* parent;
  void my_error(const QString& error) const;
  QString filename;
  [[nodiscard]] virtual QByteArray PEM_comment() const;
  virtual void collect_properties(QMap<QString, QString>&) const;
  QList<pki_base*> childItems;

 public:
  enum msg_type {
    msg_import,
    msg_delete,
    msg_delete_multi,
    msg_create,
  };
  enum print_opt {
    print_openssl_txt,
    print_pem,
    print_coloured,
  };
  enum pki_source pkiSource;

  pki_base(const QString& d = QString(), pki_base* p = nullptr);
  pki_base(const pki_base* p);
  ~pki_base() override;

  [[nodiscard]] QList<pki_base*> getChildItems() const;
  void clear();
  [[nodiscard]] QString getIntName() const { return desc; }
  void setFilename(const QString& s) { filename = s; }
  [[nodiscard]] QString getFilename() const { return filename; }
  void inheritFilename(pki_base* pki) const { pki->setFilename(getFilename()); }
  [[nodiscard]] virtual QString comboText() const;
  virtual void print(BioByteArray& b, enum print_opt opt) const;
  [[nodiscard]] QString getUnderlinedName() const;
  void setIntName(const QString& d) { desc = d; }
  virtual void autoIntName(const QString& file);
  [[nodiscard]] QString getComment() const { return comment; }
  void setComment(const QString& c) { comment = c; }
  [[nodiscard]] QVariant getSqlItemId() const { return sqlItemId; }
  [[nodiscard]] enum pki_type getType() const { return pkiType; }
  [[nodiscard]] QString getTypeString() const;
  [[nodiscard]] QString i2d_b64() const {
    return QString::fromLatin1(i2d().toBase64());
  }
  [[nodiscard]] a1time getInsertionDate() const { return insertion_date; }
  [[nodiscard]] virtual QByteArray i2d() const;
  virtual bool compare(const pki_base*) const;
  [[nodiscard]] virtual QString getMsg(msg_type msg) const;
  [[nodiscard]] virtual const char* getClassName() const;

  /* Tree View management */
  void setParent(pki_base* p);
  [[nodiscard]] pki_base* getParent() const;
  pki_base* child(int row);
  void insert(pki_base* item);
  [[nodiscard]] int childCount() const;
  void takeChild(pki_base* pki);
  pki_base* takeFirst();
  int indexOf(const pki_base* child) const;

  /* Token handling */
  virtual void deleteFromToken();
  virtual void deleteFromToken(const slotid&);
  virtual int renameOnToken(const slotid&, const QString&);

  /* Import / Export management */
  virtual bool pem(BioByteArray& b);
  virtual void fromPEM_BIO(BIO*, const QString&);
  virtual void fromPEMbyteArray(const QByteArray&, const QString&);
  virtual void fload(const QString&);
  virtual void writeDefault(const QString&) const;

  /* Qt Model-View methods */
  virtual QVariant bg_color(const dbheader* hd) const {
    (void)hd;
    return {};
  }
  virtual QVariant column_data(const dbheader* hd) const;
  virtual QVariant getIcon(const dbheader* hd) const;
  virtual QVariant column_tooltip(const dbheader* hd) const;
  virtual a1time column_a1time(const dbheader* hd) const;
  [[nodiscard]] virtual bool visible() const;
  int isVisible();
  [[nodiscard]] bool childVisible() const;

  /* SQL management methods */
  QSqlError insertSql();
  virtual QSqlError insertSqlData() { return {}; }
  QSqlError deleteSql();
  virtual QSqlError deleteSqlData() { return {}; }
  virtual void restoreSql(const QSqlRecord& rec);
  [[nodiscard]] QSqlError sqlItemNotFound(QVariant sqlId) const;
  [[nodiscard]] unsigned hash() const;
  [[nodiscard]] QString pki_source_name() const;
  [[nodiscard]] QString get_dump_filename(const QString& dirname,
                                          const QString& ext) const;
  void selfComment(QString msg);
  [[nodiscard]] QStringList icsVEVENT(const a1time& expires,
                                      const QString& summary,
                                      const QString& description) const;
  operator QString() const {
    return QString("(%1[%2]:%3)")
        .arg(getClassName())
        .arg(getSqlItemId().toString())
        .arg(getIntName());
  }
};

Q_DECLARE_METATYPE(pki_base*)

#endif  // PKI_BASE_H
