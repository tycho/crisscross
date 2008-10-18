
#include "header.h"
#include "rijndael.h"
#include "rijndael-alg-fst.h"

using namespace CrissCross::System;

#define ITERATIONS 10000000

void rijndaelSpeed(int keyBits)
{
	int       Nr, i;
	u32       rk[4 * (MAXNR + 1)];
	u8        cipherKey[256 / 8], pt[16], ct[16];
	Stopwatch sw;
	double    sec;

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
	/*
	 * The format string ends before the 2 bytes are printed,
	 * but this is intentional. It's just to ensure that
	 * the compiler doesn't optimize out the function calls
	 * during the loop above (i.e. the end result "is" used)
	 */
	printf("Encryption:\t\t\t%.1f s, %.0f MB/s\n",
	       sec, (double)ITERATIONS * 128.0 / sec / 1048576.0,
	       pt[0], ct[1]);

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
	/*
	 * The format string ends before the 2 bytes are printed,
	 * but this is intentional. See above explanation.
	 */
	printf("Decryption:\t\t\t%.1f s, %.0f MB/s\n\n",
	       sec, (double)ITERATIONS * 128.0 / sec / 1048576.0,
	       pt[0], ct[1]);
}
