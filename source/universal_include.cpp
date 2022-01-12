/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
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

			g_stderr = new Console(stderr, nullptr);
			g_stdout = new Console(stdout, nullptr);

			CrissCross::System::InitTimer();
		}

		~CrissCrossInitializer ()
		{
			delete g_stderr; g_stderr = nullptr;
			delete g_stdout; g_stdout = nullptr;
		}
	};

	// Statically initialize the library.
	CrissCrossInitializer ccinit;
}
