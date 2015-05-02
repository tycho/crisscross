
#include "header.h"
#include "rijndael.h"
#include "rijndael-alg-fst.h"

#include <cstdio>

using namespace CrissCross::System;

#define ITERATIONS 10000000

void rijndaelSpeed(int keyBits)
{
	int Nr, i;
	u32 rk[4 * (MAXNR + 1)];
	u8 cipherKey[256 / 8], pt[16], ct[16];
	Stopwatch sw;
	double sec;

	memset(cipherKey, 0, sizeof(cipherKey));

	printf("%d-bit keys\n", keyBits);

	/*
	 * Encryption key setup timing:
	 */
	sw.Start();
	for (i = 0; i < ITERATIONS; i++) {
		Nr = rijndaelKeySetupEnc(rk, cipherKey, keyBits);
	}

	sw.Stop();
	sec = sw.Elapsed();
	printf("Encryption key schedule:\t%.1f s, %.0f MB/s\n",
	       sec, (double)ITERATIONS * 128.0 / sec / 1048576.0);

	/*
	 * Encryption timing:
	 */
	sw.Start();
	for (i = 0; i < ITERATIONS; i++) {
		rijndaelEncrypt(rk, Nr, pt, ct);
	}

	sw.Stop();
	sec = sw.Elapsed();
	printf("Encryption:\t\t\t%.1f s, %.0f MB/s\n",
	       sec, (double)ITERATIONS * 128.0 / sec / 1048576.0);

	/*
	 * Decryption key setup timing:
	 */
	sw.Start();
	for (i = 0; i < ITERATIONS; i++) {
		Nr = rijndaelKeySetupDec(rk, cipherKey, keyBits);
	}

	sw.Stop();
	sec = sw.Elapsed();
	printf("Decryption key schedule:\t%.1f s, %.0f MB/s\n",
	       sec, (double)ITERATIONS * 128.0 / sec / 1048576.0);

	/*
	 * Decryption timing:
	 */
	sw.Start();
	for (i = 0; i < ITERATIONS; i++) {
		rijndaelDecrypt(rk, Nr, pt, ct);
	}

	sw.Stop();
	sec = sw.Elapsed();
	printf("Decryption:\t\t\t%.1f s, %.0f MB/s\n\n",
	       sec, (double)ITERATIONS * 128.0 / sec / 1048576.0);
}
