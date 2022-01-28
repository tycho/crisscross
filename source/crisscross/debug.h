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

#ifndef __included_cc_debug_h
#define __included_cc_debug_h

#include <crisscross/console.h>

#include <stdlib.h>

#define USE_FAST_ASSERT

#ifdef _DEBUG

/*! @cond */

#ifndef USE_FAST_ASSERT

void Assert(bool _condition, const char *_testcase, const char *_file, int _line);
#define CoreAssert(x)       Assert((x), # x, __FILE__, __LINE__)

#else

#define CoreAssert(x) { \
		if (!(x)) { \
			g_stderr->WriteLine("\nAssertion failed : '%s'\nFile: %s\nLine: %d\n", \
			                    # x, __FILE__, __LINE__); \
			assert(x); \
		} \
}

#endif
/*! @endcond */
#else
#	define CoreAssert(x) if (!(x)) do {} while (0)
#endif

#ifdef CC_ENABLE_STACK_WALKER
namespace CrissCross {
	namespace Debug {
		/*! \brief Prints a stack trace to _outputBuffer. */
		/*!
		 * \param _outputBuffer The CoreIO output buffer to write to.
		 */
		void PrintStackTrace(CrissCross::IO::CoreIOWriter * _outputBuffer);
	}
}
#endif

extern CrissCross::IO::Console *g_stderr;
extern CrissCross::IO::Console *g_stdout;

#endif
