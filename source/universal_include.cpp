/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>
#include <crisscross/rbtree.h>
#include <crisscross/llist.h>
#include <crisscross/debug.h>

#ifdef TARGET_OS_MACOSX
extern "C" int NSApplicationMain(int argc, const char *argv []);
#endif

#ifdef SDL_APPLICATION
#ifndef TARGET_OS_WINDOWS
#include <SDL/SDL.h>
#else
#define main SDL_main
#endif
#endif

using namespace std;
using namespace CrissCross::Data;
using namespace CrissCross::Debug;
using namespace CrissCross::IO;

Console     *g_stderr;
Console     *g_stdout;

#ifdef DETECT_MEMORY_LEAKS

#pragma warning ( disable: 4312 )

#ifdef ENABLE_MEMLEAK_STATS
_CrtMemState s1, s2, s3;
#endif

void ParseMemoryLeakFile(const char *_inputFilename, const char *_outputFilename)
{
	/* */
	/* Start up */
	/* */

	RedBlackTree<char *, int> combined;
	RedBlackTree<char *, int> frequency;
	int                       unrecognised = 0;

	/* */
	/* Open the file and start parsing */
	/* */

	FILE                     *memoryfile = fopen(_inputFilename, "rb");

	while (memoryfile && !feof(memoryfile))	{
		char thisline[1024];

		fgets(thisline, 1024, memoryfile);

		if (!strncmp(thisline, " Data:", 6) == 0) { /* This line is a data line - useless to us */
			if (strchr(thisline, ':')) {                               /* This line does not have a source file location - useless to us */
				/* Get the size */

				char *lastcomma = strrchr(thisline, ',');
				if (lastcomma == 0) continue;

				char *ssize = lastcomma + 2;
				int   size;
				char  unused[32];

				sscanf(ssize, "%d %s", &size, unused);

				/* Get the source file name */

				char *sourcelocation = thisline;
				char *colon = strrchr(thisline, ':');

				*(colon - 1) = '\x0';

				/* Put the result into our BTree */

				int   result = 0;
				bool  found = combined.find(sourcelocation, result);

				if (found)
					combined.replace(sourcelocation, result + size);
				else
					combined.insert(sourcelocation, size);

				found = frequency.find(sourcelocation, result);

				if (frequency.exists(sourcelocation))
					frequency.replace(sourcelocation, result + size);
				else
					frequency.insert(sourcelocation, 1);
			} else {
				char *lastcomma = strrchr(thisline, ',');

				if (lastcomma) {
					char *ssize = lastcomma + 2;
					int   size;
					char  unused[32];

					sscanf(ssize, "%d %s", &size, unused);

					unrecognised += size;
				}
			}
		}
	}

	fclose(memoryfile);


	/* */
	/* Sort the results into a list */
	/* */

	LList<char *>   sorted;
	DArray<char *> *dataI = combined.ConvertIndexToDArray();
	DArray<int>    *dataD = combined.ConvertToDArray();

	int             totalsize = 0;

	for (size_t i = 0; i < dataI->size(); i++) {
		if (dataI->valid(i)) {
			char *newsource = dataI->get(i);
			int   newsize = dataD->get(i);

			totalsize += newsize;
			bool  inserted = false;

			for (size_t i = 0; i < sorted.size(); i++) {
				char *existingsource = sorted.get(i);
				int   existingsize;

				combined.find(existingsource, existingsize);

				if (newsize <= existingsize) {
					sorted.insert_at(newsource, i);
					inserted = true;
					break;
				}
			}

			if (!inserted)
				sorted.insert(newsource);
		}
	}

	delete dataI;
	delete dataD;


	/* */
	/* Open the output file */
	/* */

	if (sorted.size()) {
		FILE *output = fopen(_outputFilename, "wt");

		/* */
		/* Print out our sorted list */
		/* */

		fprintf(output, "Total recognised memory leaks   : %d Kbytes\n",
		        int( totalsize / 1024 ));
		fprintf(output, "Total unrecognised memory leaks : %d Kbytes\n\n",
		        int( unrecognised / 1024 ));

		for (int k = (int)sorted.size() - 1; k >= 0 && k < (int)sorted.size(); k--) {
			char *source = sorted.get(k);
			CoreAssert(source);
			int   size; combined.find(source, size);
			int   freq; frequency.find(source, freq);

			if (size > 1048576) {
				fprintf(output, "%-95s (%d MB in %d leaks)\n", source,
				        int( size / 1048576 ), freq);
			} else if (size > 2048) {
				fprintf(output, "%-95s (%d KB in %d leaks)\n", source,
				        int( size / 1024 ), freq);
			} else {
				fprintf(output, "%-95s (%d  bytes in %d leaks)\n", source, size,
				        freq);
			}
		}

		/* */
		/* Clean up */

		fclose(output);
	}
}


void AppPrintMemoryLeaks(char *_filename)
{
	/* */
	/* Print all raw memory leak data to a temporary file */

#ifdef ENABLE_MEMLEAK_STATS
	_CrtMemCheckpoint(&s2);
#endif

	char     tmpFilename[512];

	sprintf(tmpFilename, "%s.tmp", _filename);

	OFSTRUCT ofstruct;
	HFILE    file = OpenFile(tmpFilename,
	                         &ofstruct,
	                         OF_CREATE);

	CoreAssert(file);

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, ( _HFILE )file);

	BOOL     memLeakFound = _CrtDumpMemoryLeaks();
#ifdef ENABLE_MEMLEAK_STATS
	_CrtMemDifference(&s3, &s1, &s2);
	_CrtMemDumpStatistics(&s3);
#endif

	_lclose(file);


	/* */
	/* Parse the temp file into a sensible format */

	if (memLeakFound)
		ParseMemoryLeakFile(tmpFilename, _filename);

	/* */
	/* Delete the temporary file */

#ifdef TARGET_OS_WINDOWS
	DeleteFileA(tmpFilename);
#else
	unlink(tmpFilename);
#endif
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef COCOA_APPLICATION
int RunApplication(int argc, char * *argv)
{
	return NSApplicationMain(argc, (const char * *)argv);
}
#endif

int main(int argc, char * *argv)
{
	int retval = 0;

#ifdef TARGET_OS_NDSFIRMWARE
	fatInitDefault();
#endif

#ifdef ENABLE_MEMLEAK_STATS
	_CrtMemCheckpoint(&s1);
#endif
	g_stderr = new Console(stderr, NULL);
	g_stdout = new Console(stdout, NULL);

#ifdef ENABLE_CRASHREPORTS
	__try
#endif
	{
#ifndef _WINDOWS
		retval = RunApplication(argc, argv);
#else
		retval = RunApplication(__argc, __argv);
#endif
	}
#ifdef ENABLE_CRASHREPORTS
	__except(RecordExceptionInfo(GetExceptionInformation(), "WinMain", CC_LIB_NAME, CC_LIB_VERSION)){
	}
#endif

	delete g_stderr; g_stderr = NULL;
	delete g_stdout; g_stdout = NULL;

#ifdef DETECT_MEMORY_LEAKS
	AppPrintMemoryLeaks("memleak.txt");
#endif

	return retval;
}

#ifdef TARGET_OS_WINDOWS
#ifndef SDL_APPLICATION
int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _cmdLine, int _iCmdShow)
{
	main(__argc, __argv);
}
#endif
#endif

#ifdef __cplusplus
}
#endif
