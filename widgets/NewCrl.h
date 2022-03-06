#ifndef __NEWCRL_H
#define __NEWCRL_H

class pki_x509;
class QWidget;

class NewCrl {
 public:
  static void newCrl(QWidget* parent, pki_x509* issuer);
};

#endif
