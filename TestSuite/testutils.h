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

#ifndef __included_testutils_h
#define __included_testutils_h

#include <crisscross/console.h>
#include <crisscross/debug.h>

extern CrissCross::IO::Console *g_console;

#define VERBOSE

#ifdef VERBOSE
#define TEST_ASSERT(condition) \
        if (!(condition)) { \
		g_console->SetColour(g_console->FG_YELLOW | g_console->FG_INTENSITY); \
		g_console->WriteLine("\nWARNING: Condition '%s' failed assertion!\nFile: '%s' Line: %d", \
			#condition, __FILE__, __LINE__); \
		g_console->SetColour(g_console->FG_RED | g_console->FG_INTENSITY); \
		CrissCross::Debug::PrintStackTrace(g_console); \
		g_console->SetColour(); \
		return 1; \
	}

#else
#define TEST_ASSERT(condition) \
        if (!(condition)) { \
		return 1; \
	}

#endif

void WritePrefix(const char *_prefix);
int WriteResult(int _result);
char *newStr(const char *_string);

#endif
