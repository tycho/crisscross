/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"

#include <cstdio>

using namespace CrissCross::Data;
using namespace CrissCross::IO;
using namespace CrissCross::System;
using namespace std;

Console *console = NULL;

#ifdef ENABLE_SORTS
void BenchmarkDArray(Sorter<char *> &sorter)
{
	DArray<char *> data, rdata;
	Stopwatch sw;
	char buffer[512], format[64];
	sprintf(format, "%s", "%4.3lfs");

	FileReader file;

	file.SetLineEndings(CC_LN_LF);
	file.Open("dataset");

	if (file.IsOpen()) {
		data.setStepDouble();
		rdata.setStepDouble();

		console->Write("Loading... ");

		sw.Start();

		/* Load the file into the data DArray */
		while (file.ReadLine(buffer, sizeof(buffer)) >= 0)
			data.insert(cc_strdup(buffer));

		sw.Stop();
		console->WriteLine(format, sw.Elapsed());

		file.Close();

		console->WriteLine("Loaded %d items.", data.used());

		console->Write("Random: ");
		sw.Start();
		data.sort(sorter);
		sw.Stop();
		console->WriteLine(format, sw.Elapsed());

		/* Create a reverse-sorted DArray */
		for (long i = (long)data.size(); i >= 0; i--) {
			if (data.valid(i)) {
				rdata.insert(data.get(i));
			}
		}

		console->Write("Pre-sorted: ");
		sw.Start();
		data.sort(sorter);
		sw.Stop();
		console->WriteLine(format, sw.Elapsed());

		console->Write("Reverse-sorted: ");
		sw.Start();
		rdata.sort(sorter);
		sw.Stop();
		console->WriteLine(format, sw.Elapsed());

		for (size_t i = 0; i < data.size(); i++) {
			if (data.valid(i)) {
				free(data.get(i));
				data.remove(i);
			}
		}

		data.empty();
		rdata.empty();
	} else {
		console->WriteLine("Dataset not found.");
	}
}
#endif

int main(int argc, char * *argv)
{
	console = new Console();

#ifdef ENABLE_SORTS

	QuickSort<char *>     qs;
	HeapSort<char *>      hs;
	CombSort<char *>      cs;
	ShellSort<char *>     ss;

#ifdef TARGET_OS_NDSFIRMWARE
	chdir("/data/SortBenchmark/");
#endif
	console->SetColour(console->FG_BLUE | console->FG_INTENSITY);
	console->WriteLine("HeapSort...");
	console->SetColour();
	BenchmarkDArray(hs);
	console->WriteLine();
	console->SetColour(console->FG_BLUE | console->FG_INTENSITY);
	console->WriteLine("CombSort...");
	console->SetColour();
	BenchmarkDArray(cs);
	console->WriteLine();
	console->SetColour(console->FG_BLUE | console->FG_INTENSITY);
	console->WriteLine("ShellSort...");
	console->SetColour();
	BenchmarkDArray(ss);
	console->WriteLine();
	console->SetColour(console->FG_BLUE | console->FG_INTENSITY);
	console->WriteLine("QuickSort...");
	console->SetColour();
	BenchmarkDArray(qs);
	console->WriteLine();
#else
	console->WriteLine("Sorting algorithms are disabled in this build of CrissCross.\nDefine ENABLE_SORTS in universal_include.h if you want them.");
#endif

	console->WriteLine();

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
