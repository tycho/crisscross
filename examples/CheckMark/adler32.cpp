/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2020 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"

/*
 * This implementation of Adler-32 was taken and modified from zlib
 * to work with standard C++ Copyright notices below...
 *
 */

/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-2004 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* zlib version 1.2.3, July 18th, 2005
 *
 * Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *   claim that you wrote the original software. If you use this software
 *   in a product, an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *   misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Jean-loup Gailly        Mark Adler
 * jloup@gzip.org          madler@alumni.caltech.edu
 *
 *
 * The data format used by the zlib library is described by RFCs (Request for
 * Comments) 1950 to 1952 in the files http://www.ietf.org/rfc/rfc1950.txt
 * (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
 */

#define BASE 65521UL    /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf, i)  {adler += (buf)[i]; sum2 += adler;}
#define DO2(buf, i)  DO1(buf, i); DO1(buf, i + 1);
#define DO4(buf, i)  DO2(buf, i); DO2(buf, i + 2);
#define DO8(buf, i)  DO4(buf, i); DO4(buf, i + 4);
#define DO16(buf)   DO8(buf, 0); DO8(buf, 8);

/* use NO_DIVIDE if your processor does not do division in hardware */
#ifdef NO_DIVIDE
#define MOD(a) \
        do { \
		if (a >= (BASE << 16)) a -= (BASE << 16);\
		if (a >= (BASE << 15)) a -= (BASE << 15);\
		if (a >= (BASE << 14)) a -= (BASE << 14);\
		if (a >= (BASE << 13)) a -= (BASE << 13);\
		if (a >= (BASE << 12)) a -= (BASE << 12);\
		if (a >= (BASE << 11)) a -= (BASE << 11);\
		if (a >= (BASE << 10)) a -= (BASE << 10);\
		if (a >= (BASE << 9)) a -= (BASE << 9);\
		if (a >= (BASE << 8)) a -= (BASE << 8);\
		if (a >= (BASE << 7)) a -= (BASE << 7);\
		if (a >= (BASE << 6)) a -= (BASE << 6);\
		if (a >= (BASE << 5)) a -= (BASE << 5);\
		if (a >= (BASE << 4)) a -= (BASE << 4);\
		if (a >= (BASE << 3)) a -= (BASE << 3);\
		if (a >= (BASE << 2)) a -= (BASE << 2);\
		if (a >= (BASE << 1)) a -= (BASE << 1);\
		if (a >= BASE) a -= BASE;\
	} while (0)
#define MOD4(a) \
        do { \
		if (a >= (BASE << 4)) a -= (BASE << 4);\
		if (a >= (BASE << 3)) a -= (BASE << 3);\
		if (a >= (BASE << 2)) a -= (BASE << 2);\
		if (a >= (BASE << 1)) a -= (BASE << 1);\
		if (a >= BASE) a -= BASE;\
	} while (0)
#else
#define MOD(a) a %= BASE
#define MOD4(a) a %= BASE
#endif

/* ========================================================================= */
unsigned long adler32(unsigned long adler, const char *buf, size_t len)
{
	unsigned long sum2;
	unsigned n;

	/* split Adler-32 into component sums */
	sum2 = (adler >> 16) & 0xffff;
	adler &= 0xffff;

	/* in case user likes doing a byte at a time, keep it fast */
	if (len == 1) {
		adler += buf[0];
		if (adler >= BASE)
			adler -= BASE;

		sum2 += adler;
		if (sum2 >= BASE)
			sum2 -= BASE;

		return adler | (sum2 << 16);
	}

	/* initial Adler-32 value (deferred check for len == 1 speed) */
	if (*buf == '\0')
		return 1L;

	/* in case short lengths are provided, keep it somewhat fast */
	if (len < 16) {
		while (len--) {
			adler += *buf++;
			sum2 += adler;
		}
		if (adler >= BASE)
			adler -= BASE;

		MOD4(sum2);                /* only added so many BASE's */
		return adler | (sum2 << 16);
	}

	/* do length NMAX blocks -- requires just one modulo operation */
	while (len >= NMAX) {
		len -= NMAX;
		n = NMAX / 16;          /* NMAX is divisible by 16 */
		do {
			DO16(buf);             /* 16 sums unrolled */
			buf += 16;
		} while (--n);
		MOD(adler);
		MOD(sum2);
	}

	/* do remaining bytes (less than NMAX, still just one modulo) */
	if (len) {                   /* avoid modulos if none remaining */
		while (len >= 16) {
			len -= 16;
			DO16(buf);
			buf += 16;
		}
		while (len--) {
			adler += *buf++;
			sum2 += adler;
		}
		MOD(adler);
		MOD(sum2);
	}

	/* return recombined sums */
	return adler | (sum2 << 16);
}
