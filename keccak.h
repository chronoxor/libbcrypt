#ifndef KECCAK_H_
#define KECCAK_H_

typedef unsigned char u8;
typedef unsigned long long int u64;
typedef unsigned int ui;

void FIPS202_SHAKE128(const u8 *in, u64 inLen, u8 *out, u64 outLen);
void FIPS202_SHAKE256(const u8 *in, u64 inLen, u8 *out, u64 outLen);
void FIPS202_SHA3_224(const u8 *in, u64 inLen, u8 *out);
void FIPS202_SHA3_256(const u8 *in, u64 inLen, u8 *out);
void FIPS202_SHA3_384(const u8 *in, u64 inLen, u8 *out);
void FIPS202_SHA3_512(const u8 *in, u64 inLen, u8 *out);

#endif
