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

using namespace CrissCross::Data;
using namespace CrissCross::IO;
using namespace CrissCross::System;
using namespace std;

Console *console = NULL;

void BenchmarkDArray(Sorter<char *> &sorter)
{
	DArray<char *> data, rdata;
	Stopwatch      sw;
	char           buffer[512], format[64];
	sprintf(format, "%s", "%4.3lfs");

	FileReader     file;

	file.SetLineEndings(CC_LN_LF);
	file.Open("dataset");

	if (file.IsOpen()) {
		data.setStepDouble();
		rdata.setStepDouble();

		console->Write("Loading... ");

		sw.Start();

		/* Load the file into the data DArray */
		while (file.ReadLine(buffer, sizeof(buffer)))
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

int RunApplication(int argc, char * *argv)
{
	console = new Console();

#if !defined(DISABLE_DEPRECATED_CODE)
	BubbleSort<char *>    bs;
	QuickSort<char *>     qs;
	InsertionSort<char *> is;
#endif
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
#if !defined(DISABLE_DEPRECATED_CODE)
	console->SetColour(console->FG_BLUE | console->FG_INTENSITY);
	console->WriteLine("QuickSort...");
	console->SetColour();
	BenchmarkDArray(qs);
	console->WriteLine();
	console->SetColour(console->FG_BLUE | console->FG_INTENSITY);
	console->WriteLine("BubbleSort...");
	console->SetColour();
	BenchmarkDArray(bs);
	console->WriteLine();
	console->SetColour(console->FG_BLUE | console->FG_INTENSITY);
	console->WriteLine("InsertionSort...");
	console->SetColour();
	BenchmarkDArray(is);
	console->WriteLine();
#endif

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
