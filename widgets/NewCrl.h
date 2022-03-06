#ifndef NEW_CRL_H
#define NEW_CRL_H

class pki_x509;
class QWidget;

class NewCrl {
 public:
  static void newCrl(QWidget* parent, pki_x509* issuer);
};

#endif  // NEW_CRL_H
