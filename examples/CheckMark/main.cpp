/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of IO.IN Research.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"

#include "adler32.h"
#include "crc32.h"
#include "random.h"

using namespace CrissCross::IO;
using namespace CrissCross::System;
using namespace std;

int RunApplication(int argc, char * *argv)
{
	Console                         *console = new Console();

	/* Begin your application here. */

	CrissCross::Data::DArray<char *> randomStrings;
	CrissCross::System::Stopwatch    sw;

	console->SetColour(console->FG_RED | console->FG_INTENSITY);
	console->WriteLine("CheckMark v" CC_LIB_VERSION);
	console->SetColour();
	console->WriteLine("A checksum algorithm benchmark.");
	console->WriteLine();

	/* Generate some data for checksum speed tests. */
	console->Write("Generating data... ");
	sw.Start();
	GenerateRandom(randomStrings);
	sw.Stop();
	console->WriteLine("%5.3lfs", sw.Elapsed());
	console->WriteLine();

	/* This variable just makes the compiler think */
	/* the data is being used somehow. */
	unsigned long killOptimization = 0;

	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++) {
			killOptimization = adler32(0, randomStrings.get(i), ENTRY_LENGTH);
		}

	sw.Stop();
	console->WriteLine("%8lu AdlerMarks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()),
	                   killOptimization);

	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++) {
			killOptimization = crc32(randomStrings.get(i), ENTRY_LENGTH);
		}

	sw.Stop();
	console->WriteLine("%8lu CRCMarks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()),
	                   killOptimization);

	/* It's too slow. Disabled. */
#if 0
	CrissCross::Crypto::MD2Hash    md2;
	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++)
			md2.Process(randomStrings.get(i), ENTRY_LENGTH);

	sw.Stop();
	console->WriteLine("%8lu MD2Marks",
	                   (unsigned long)(((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()) / 1024.0));
#endif


	CrissCross::Crypto::MD4Hash    md4;
	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++)
			md4.Process(randomStrings.get(i), ENTRY_LENGTH);

	sw.Stop();
	console->WriteLine("%8lu MD4Marks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()));

	CrissCross::Crypto::MD5Hash    md5;
	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++)
			md5.Process(randomStrings.get(i), ENTRY_LENGTH);

	sw.Stop();
	console->WriteLine("%8lu MD5Marks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()));

	CrissCross::Crypto::SHA1Hash   sha1;
	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++)
			sha1.Process(randomStrings.get(i), ENTRY_LENGTH);

	sw.Stop();
	console->WriteLine("%8lu SHA1Marks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()));

	CrissCross::Crypto::SHA256Hash sha256;
	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++)
			sha256.Process(randomStrings.get(i), ENTRY_LENGTH);

	sw.Stop();
	console->WriteLine("%8lu SHA256Marks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()));

	CrissCross::Crypto::SHA512Hash sha512;
	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++)
			sha512.Process(randomStrings.get(i), ENTRY_LENGTH);

	sw.Stop();
	console->WriteLine("%8lu SHA512Marks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()));

	CrissCross::Crypto::TigerHash  tiger;
	sw.Start();
	for (int r = 0; r < MAX_RUNS; r++)
		for (int i = 0; i < DATASET_SIZE; i++)
			tiger.Process(randomStrings.get(i), ENTRY_LENGTH);

	sw.Stop();
	console->WriteLine("%8lu TigerMarks",
	                   (unsigned long)((double)(DATASET_SIZE * MAX_RUNS) / sw.Elapsed()));


	randomStrings.flushArray();
	console->WriteLine();

	/* End your application here. */

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
