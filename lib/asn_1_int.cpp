#include "asn_1_int.h"

#include "exception.h"
#include "func.h"

#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/err.h>

#include <memory>

static void OpenSslFree(char* ptr) noexcept { OPENSSL_free(ptr); }

template <auto DeleteFn>
struct FunctionDeleter {
  template <class T>
  void operator()(T* ptr) {
    DeleteFn(ptr);
  }
};

void Asn1IntegerDeleter::operator()(ASN1_INTEGER* ptr) {
  ASN1_INTEGER_free(ptr);
}

using BigNum = std::unique_ptr<BIGNUM, FunctionDeleter<BN_free>>;
using SslString = std::unique_ptr<char, FunctionDeleter<OpenSslFree>>;

[[nodiscard]] static auto MakeUnique(BIGNUM* ptr) noexcept {
  return BigNum{ptr};
}

[[nodiscard]] static BigNum Asn1IntegerToBn(const Asn1Integer& in) noexcept {
  return MakeUnique(ASN1_INTEGER_to_BN(in.get(), nullptr));
}

[[nodiscard]] static SslString BigNumToDec(const BigNum& bn) noexcept {
  return SslString{BN_bn2dec(bn.get())};
}

[[nodiscard]] static SslString BigNumToHex(const BigNum& bn) noexcept {
  return SslString{BN_bn2hex(bn.get())};
}

static void Asn1IntegerSet(Asn1Integer& a, long v) {
  ASN1_INTEGER_set(a.get(), v);
}

[[nodiscard]] static int Asn1IntegerCmp(const Asn1Integer& x,
                                        const Asn1Integer& y) noexcept {
  return ASN1_INTEGER_cmp(x.get(), y.get());
}

ASN1_INTEGER* a1int::dup(const ASN1_INTEGER* a) {
  // this wrapper casts the const to work around the nonconst
  // declared ASN1_STRING_dup (actually it is const
  ASN1_INTEGER* r = ASN1_INTEGER_dup(const_cast<ASN1_INTEGER*>(a));
  openssl_error();
  if (!r) {
    r = ASN1_INTEGER_new();
  }
  Q_CHECK_PTR(r);
  return r;
}

a1int::a1int() : in{ASN1_INTEGER_new()} {
  Q_CHECK_PTR(in.get());

  Asn1IntegerSet(in, 0);
  openssl_error();
}

a1int::a1int(const ASN1_INTEGER* i) : in{dup(i)} {}

a1int::a1int(const a1int& a) : in{dup(a.in.get())} {}

a1int::a1int(const QString& hex) : in{ASN1_INTEGER_new()} {
  Q_CHECK_PTR(in.get());
  setHex(hex);
}

a1int::~a1int() = default;

a1int::a1int(long l) : in{ASN1_INTEGER_new()} {
  Q_CHECK_PTR(in.get());
  set(l);
}

a1int& a1int::set(const ASN1_INTEGER* i) {
  in.reset(dup(i));
  return *this;
}

a1int& a1int::set(long l) {
  Asn1IntegerSet(in, l);

  openssl_error();
  return *this;
}

QString a1int::toQString(int dec) const {
  if (in->length == 0) {
    return {};
  }
  const auto bn = Asn1IntegerToBn(in);
  openssl_error();

  const auto res = dec ? BigNumToDec(bn) : BigNumToHex(bn);
  openssl_error();
  return {res.get()};
}

QString a1int::toHex() const { return toQString(0); }

QString a1int::toDec() const { return toQString(1); }

a1int& a1int::setQString(const QString& s, int dec) {
  BIGNUM* bn = nullptr;
  if (s.isEmpty()) {
    return *this;
  }
  if (dec) {
    BN_dec2bn(&bn, s.toLatin1());
  } else {
    BN_hex2bn(&bn, s.toLatin1());
  }
  openssl_error();

  BN_to_ASN1_INTEGER(bn, in.get());  // TODO(melg): add functions for BigNum.
  openssl_error();
  BN_free(bn);
  return *this;
}

a1int& a1int::setHex(const QString& s) { return setQString(s, 0); }

a1int& a1int::setDec(const QString& s) { return setQString(s, 1); }

a1int& a1int::setRaw(const unsigned char* data, unsigned len) {
  BIGNUM* bn = BN_bin2bn(data, len, nullptr);
  if (!bn) {
    openssl_error();
  }
  BN_to_ASN1_INTEGER(bn, in.get());  // TODO(melg): add functions for BigNum.
  openssl_error();
  BN_free(bn);
  return *this;
}

ASN1_INTEGER* a1int::get() const { return dup(in.get()); }

const ASN1_INTEGER* a1int::get0() const { return in.get(); }

long a1int::getLong() const {
  long l = ASN1_INTEGER_get(in.get());
  openssl_error();
  return l;
}

a1int& a1int::operator++() {
  BIGNUM* bn = ASN1_INTEGER_to_BN(in.get(), nullptr);

  openssl_error();
  BN_add(bn, bn, BN_value_one());

  openssl_error();
  BN_to_ASN1_INTEGER(bn, in.get());
  openssl_error();
  BN_free(bn);
  return *this;
}

a1int a1int::operator++(int) {
  a1int tmp = *this;
  operator++();
  return tmp;
}

a1int& a1int::operator=(const a1int& a) {
  set(a.in.get());  // TODO(melg): maybe set need unique ptr and not just ptr.
  return *this;
}

a1int& a1int::operator=(long i) {
  ASN1_INTEGER_set(in.get(), i);
  openssl_error();
  return *this;
}

bool a1int::operator>(const a1int& a) const {
  return Asn1IntegerCmp(in, a.in) > 0;
}

bool a1int::operator<(const a1int& a) const {
  return Asn1IntegerCmp(in, a.in) < 0;
}

bool a1int::operator==(const a1int& a) const {
  return Asn1IntegerCmp(in, a.in) == 0;
}

bool a1int::operator!=(const a1int& a) const {
  return Asn1IntegerCmp(in, a.in) != 0;
}

// a1int::operator QString() const { return toHex(); }

QByteArray a1int::i2d() {
  return i2d_bytearray(I2D_VOID(i2d_ASN1_INTEGER), in.get());
}

int a1int::derSize() const { return i2d_ASN1_INTEGER(in.get(), nullptr); }
