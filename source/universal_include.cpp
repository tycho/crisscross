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

#include <crisscross/universal_include.h>
#include <crisscross/rbtree.h>
#include <crisscross/llist.h>
#include <crisscross/debug.h>
#include <crisscross/system.h>

using namespace std;
using namespace CrissCross::Data;
using namespace CrissCross::Debug;
using namespace CrissCross::IO;

Console *g_stderr;
Console *g_stdout;

namespace {
#ifdef DETECT_MEMORY_LEAKS
	_CrtMemState s1;

	void ParseMemoryLeakFile(const char *_inputFilename, const char *_outputFilename)
	{
		/* */
		/* Start up */
		/* */

		RedBlackTree<char *, int> combined;
		RedBlackTree<char *, int> frequency;
		int unrecognised = 0;

		/* */
		/* Open the file and start parsing */
		/* */

		FILE *memoryfile = fopen(_inputFilename, "rb");

		if (!memoryfile)
			return;

		while (memoryfile && !feof(memoryfile))	{
			char thisline[1024];

			fgets(thisline, 1024, memoryfile);

			if (!strncmp(thisline, " Data:", 6) == 0) { /* This line is a data line - useless to us */
				if (strchr(thisline, ':')) {                               /* This line does not have a source file location - useless to us */
					/* Get the size */

					char *lastcomma = strrchr(thisline, ',');
					if (lastcomma == 0) continue;

					char *ssize = lastcomma + 2;
					int size;
					char unused[32];

					sscanf(ssize, "%d %s", &size, unused);

					/* Get the source file name */

					char *sourcelocation = thisline;
					char *colon = strrchr(thisline, ':');

					*(colon - 1) = '\x0';

					/* Put the result into our BTree */

					int result = combined.find(sourcelocation, -1);

					if (result != -1)
						combined.replace(sourcelocation, result + size);
					else
						combined.insert(sourcelocation, size);

					result = frequency.find(sourcelocation, -1);

					if (result != -1)
						frequency.replace(sourcelocation, result + size);
					else
						frequency.insert(sourcelocation, 1);
				} else {
					char *lastcomma = strrchr(thisline, ',');

					if (lastcomma) {
						char *ssize = lastcomma + 2;
						int size;
						char unused[32];

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
		DArray<int> *dataD = combined.ConvertToDArray();

		int totalsize = 0;

		for (size_t i = 0; i < dataI->size(); i++) {
			if (dataI->valid(i)) {
				char *newsource = dataI->get(i);
				int newsize = dataD->get(i);

				totalsize += newsize;
				bool inserted = false;

				for (size_t i = 0; i < sorted.size(); i++) {
					char *existingsource = sorted.get(i);

					int existingsize = combined.find(existingsource, -1);

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
					int (totalsize / 1024));
			fprintf(output, "Total unrecognised memory leaks : %d Kbytes\n\n",
					int (unrecognised / 1024));

			for (size_t k = sorted.size() - 1; k < sorted.size(); k--) {
				char *source = sorted.get(k);
				CoreAssert(source);
				int size = combined.find(source, -1);
				int freq = frequency.find(source, -1);

				if (size > 1048576) {
					fprintf(output, "%-95s (%d MB in %d leaks)\n", source,
							int (size / 1048576), freq);
				} else if (size > 2048) {
					fprintf(output, "%-95s (%d KB in %d leaks)\n", source,
							int (size / 1024), freq);
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

		char tmpFilename[512];

		sprintf(tmpFilename, "%s.tmp", _filename);

		OFSTRUCT ofstruct;
		HFILE file = OpenFile(tmpFilename,
							  &ofstruct,
							  OF_CREATE);

		CoreAssert(file);

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, ( _HFILE )file);

		_CrtMemDumpAllObjectsSince(&s1);

		_lclose(file);


		/* */
		/* Parse the temp file into a sensible format */
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
	class CrissCrossInitializer
	{
	public:
		CrissCrossInitializer ()
		{
#ifdef TARGET_OS_NDSFIRMWARE
			fatInitDefault();
#endif

#ifdef DETECT_MEMORY_LEAKS
			_CrtMemCheckpoint(&s1);
#endif

			g_stderr = new Console(stderr, NULL);
			g_stdout = new Console(stdout, NULL);

			CrissCross::System::InitTimer();
		}

		~CrissCrossInitializer ()
		{
			delete g_stderr; g_stderr = NULL;
			delete g_stdout; g_stdout = NULL;

#ifdef DETECT_MEMORY_LEAKS
			AppPrintMemoryLeaks("memleak.txt");
#endif
		}
	};

	// Statically initialize the library.
	CrissCrossInitializer ccinit;
}
