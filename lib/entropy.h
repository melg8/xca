#ifndef ENTROPY_H
#define ENTROPY_H

#include <QByteArray>
#include <QElapsedTimer>
#include <QString>

class Entropy {
 protected:
  QString rnd;
  static QElapsedTimer timer;
  static unsigned char pool[512];
  static unsigned pool_pos;
  static unsigned seed_strength;
  static int random_from_file(QString fname, unsigned amount, int weakness = 1);

 public:
  Entropy();
  ~Entropy();
  static void add(int rand);
  static void add_buf(const unsigned char* buf, int buflen);
  static int get(unsigned char* buf, int num);
  static void seed_rng();
  static unsigned strength();
  static QString makeSalt();
};

#endif  // ENTROPY_H
