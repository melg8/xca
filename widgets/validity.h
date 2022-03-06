#ifndef VALIDITY_H
#define VALIDITY_H

#include <QDateTimeEdit>

#include <lib/asn1time.h>

class Validity final : public QDateTimeEdit {
  Q_OBJECT

  bool midnight, endDate;
  void updateFormatString();

 protected:
  QTime mytime;

 public:
  Validity(QWidget* parent);
  ~Validity() final;
  a1time getDate() const;
  void setDate(const a1time& a);
  void setDiff(const Validity* start, int number, int range);
  void hideTime(bool hide);
  void setEndDate(bool ed) { endDate = ed; }
 protected slots:
  void setMyTime(const QTime& time);

 public slots:
  void setNow();
  void hideTimeCheck(int state);
  void localTime(int);
};

#endif  // VALIDITY_H
