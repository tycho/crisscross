/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2015 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>

#include <cstdio>

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
	class CrissCrossInitializer
	{
	public:
		CrissCrossInitializer ()
		{
#ifdef TARGET_OS_NDSFIRMWARE
			fatInitDefault();
#endif

			g_stderr = new Console(stderr, NULL);
			g_stdout = new Console(stdout, NULL);

			CrissCross::System::InitTimer();
		}

		~CrissCrossInitializer ()
		{
			delete g_stderr; g_stderr = NULL;
			delete g_stdout; g_stdout = NULL;
		}
	};

	// Statically initialize the library.
	CrissCrossInitializer ccinit;
}
