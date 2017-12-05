/*
 * bcrypt wrapper library
 *
 * Written in 2011, 2013, 2014, 2015, 2017 by Ricardo Garcia <r@rg3.name>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "bcrypt.h"
#include "crypt_blowfish/ow-crypt.h"
#include "sha512.h"
#include "keccak.h"
#include "base64.h"

#define RANDBYTES (16)
#define BYTES_IN_512BITS (64)
#define BCRYPT_API __attribute__ ((visibility ("default")))

/*
 * This is a best effort implementation. Nothing prevents a compiler from
 * optimizing this function and making it vulnerable to timing attacks, but
 * this method is commonly used in crypto libraries like NaCl.
 *
 * Return value is zero if both strings are equal and nonzero otherwise.
*/
static int timing_safe_strcmp(const char *str1, const char *str2)
{
	const unsigned char *a;
	const unsigned char *b;
	int ret;
	int i;

	int len1 = strlen(str1);
	int len2 = strlen(str2);

	/* In our context both strings should always have the same length
	 * because they will be hashed passwords. */
	if (len1 != len2)
		return 1;

	/* Force unsigned for bitwise operations. */
	a = (const unsigned char *)str1;
	b = (const unsigned char *)str2;

	ret = 0;
	for (i = 0; i < len1; ++i)
		ret |= (a[i] ^ b[i]);

	return ret;
}

BCRYPT_API int bcrypt_gensalt(int factor, char salt[BCRYPT_HASHSIZE])
{
	int ret;
	char input[RANDBYTES];
	int workf;
	char *aux;

	/* Get random bytes. */
	ret = getentropy(input, sizeof(input));
	if (ret != 0)
	    return 1;

	/* Generate salt. */
	workf = (factor < 4 || factor > 31)?BCRYPT_DEFAULT_WORK_FACTOR:factor;
	aux = crypt_gensalt_rn("$2a$", workf, input, RANDBYTES,
			       salt, BCRYPT_HASHSIZE);
	return (aux == NULL)?2:0;
}

BCRYPT_API int bcrypt_hashpw(const char *passwd, const char salt[BCRYPT_HASHSIZE], char hash[BCRYPT_HASHSIZE])
{
	char *aux;
	aux = crypt_rn(passwd, salt, hash, BCRYPT_HASHSIZE);
	return (aux == NULL)?1:0;
}

BCRYPT_API int bcrypt_checkpw(const char *passwd, const char hash[BCRYPT_HASHSIZE])
{
	int ret;
	char outhash[BCRYPT_HASHSIZE];

	ret = bcrypt_hashpw(passwd, hash, outhash);
	if (ret != 0)
		return -1;

	return timing_safe_strcmp(hash, outhash);
}

BCRYPT_API int bcrypt_sha512_base64(const char *in, char digest[BCRYPT_512BITS_BASE64_SIZE])
{
	unsigned char bindigest[BYTES_IN_512BITS];
	size_t len = strlen(in);

	if (len > ULONG_MAX)
		return 1;

	sha512_calc(in, (unsigned long)len, (char *)bindigest);
	base64_calc(bindigest, sizeof(bindigest), digest);
	return 0;
}

BCRYPT_API int bcrypt_sha3_512_base64(const char *in, char digest[BCRYPT_512BITS_BASE64_SIZE])
{
	unsigned char bindigest[BYTES_IN_512BITS];
	size_t len = strlen(in);

	if(len > ULLONG_MAX)
		return 1;

	FIPS202_SHA3_512((const u8 *)in, (u64)len, bindigest);
	base64_calc(bindigest, sizeof(bindigest), digest);
	return 0;
}
