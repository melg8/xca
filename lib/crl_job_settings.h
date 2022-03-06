#ifndef CRL_JOB_SETTINGS_H
#define CRL_JOB_SETTINGS_H

#include "asn_1_int.h"
#include "asn_1_time.h"
#include "digest.h"

struct CrlJobSettings {
  bool withReason;
  bool authKeyId;
  bool subAltName;
  bool setCrlNumber;
  a1int crlNumber;
  digest hashAlgo;
  a1time lastUpdate;
  a1time nextUpdate;
};

#endif  // CRL_JOB_SETTINGS_H
