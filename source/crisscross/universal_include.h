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

#ifndef __included_cc_universal_include_h
#define __included_cc_universal_include_h

#ifdef _MSC_VER
#pragma warning (disable:4530)
#endif

#include <crisscross/build_number.h>

#ifndef SDL_APPLICATION
/* #define SDL_APPLICATION */ /* Define if your application uses SDLmain. */
#endif

#ifndef COCOA_APPLICATION
/* #  define COCOA_APPLICATION */ /* Define if your application uses NSApplicationMain */
#endif

#define CC_LIB_NAME                "CrissCross"

/* #  define CC_LIB_CODENAME            "Technetium"    // v0.1.0 codename */
/* #  define CC_LIB_CODENAME            "Freya"         // v0.2.0 codename */
/* #  define CC_LIB_CODENAME            "Hodur"         // v0.3.0 codename */
/* #  define CC_LIB_CODENAME            "Baldur"        // v0.4.0 codename */
/* #  define CC_LIB_CODENAME            "Loki"          // v0.5.0 codename */
/* #  define CC_LIB_CODENAME            "Ragnarok"      // v0.6.0 codename */
#define CC_LIB_CODENAME            "Heimdall"      /* v0.7.0 codename */
/* #  define CC_LIB_CODENAME            "Odin"          // v0.8.0 codename */
/* #  define CC_LIB_CODENAME            "Thor"          // v0.9.0 codename */
/* #  define CC_LIB_CODENAME            "Valhalla"      // v1.0.0 codename */

#define CC_LIB_VERSION             "0.7.2"
#define CC_LIB_BRANCH_AT_VERSION   "0.8.0"
#define CC_LIB_VERSION_MAJOR       0
#define CC_LIB_VERSION_MINOR       7
#define CC_LIB_VERSION_REVISION    2
const int CC_LIB_VERSION_BUILD = BUILD_NUMBER;
#undef BUILD_NUMBER
#undef __included_build_number_h

#define CC_LIB_URL                 "http://www.uplinklabs.net/crisscross/"
#define CC_LIB_COPYRIGHT           "(c) 2006-2008 by Uplink Laboratories. Licensed under the New BSD License."
#define CC_LIB_NDS_COPYRIGHT       "(c) by Uplink Laboratories.\nLicensed under the BSD License."

/* Disabling these will save space but limit functionality. */
#define ENABLE_CPUID
#define ENABLE_HASHES
#define ENABLE_SORTS
/* #define ENABLE_CRASHREPORTS / * Enables XCrashReports on Windows. * / */
/* #define DISABLE_DEPRECATED_CODE */ /* This will be enabled by default in a future release */

/* NOTE: By disabling this line, you will not be in compliance with the New BSD License. */
/*      If you disable this line, you must display the copyright notice in the program */
/*      elsewhere. */
#define ENABLE_CREDITS

/* Sorry, this is Windows-only... (Use Valgrind on Linux) */
#define DETECT_MEMORY_LEAKS

/* Don't use it. It doesn't do anything except potentially screw up memory leak detection. */
/* # define ENABLE_MEMLEAK_STATS */

/* Linux backtrace() */
#define ENABLE_BACKTRACE

/* Case sensitive Compare() function */
/* #    define CASE_SENSITIVE_COMPARE */

/* ============================================================================ */
/* Dont edit anything below this line */
/* ============================================================================ */

#define MAX_PROCESSORS 8

#include <crisscross/platform_detect.h>

/* ----------------------- */
/* RESULTANT CONFIGURATION */
/* ----------------------- */

#if defined (TARGET_OS_WINDOWS)
#define _WIN32_WINNT 0x0500   /* Windows 2000 */
#define _WIN32_IE_   0x0550   /* IE 5.5 */
#endif

#if defined (SDL_APPLICATION)
#if !defined (_CONSOLE)
#undef  _WINDOWS
#define _CONSOLE
#endif
#endif

#if defined (_OPENMP)
#include <omp.h>
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#if (!defined (TARGET_COMPILER_VC) && !defined (TARGET_COMPILER_ICC)) || defined (NO_DETECT_MEMORY_LEAKS)
#undef DETECT_MEMORY_LEAKS
#endif

#if (!defined (TARGET_OS_NDSFIRMWARE))
#define HAS_FPOS64
#endif

#if (defined (TARGET_OS_NDSFIRMWARE))
#define CASE_SENSITIVE_COMPARE
#undef ENABLE_BACKTRACE
#endif

#if (defined (__STRICT_ANSI__))
/* #  define CASE_SENSITIVE_COMPARE */
#endif

/* Doesn't work on non-x86, and Cygwin doesn't have the functionality for cpu_set_t. */
#if !(defined (TARGET_CPU_X86) || defined (TARGET_CPU_X64)) || defined (TARGET_COMPILER_CYGWIN) || defined (TARGET_OS_FREEBSD) || defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || defined (TARGET_OS_MACOSX)
#undef ENABLE_CPUID
#endif

#if !(defined (TARGET_CPU_X86) && defined (TARGET_OS_WINDOWS))
#undef ENABLE_CRASHREPORTS
#endif

/* FIXME: This is disabled due to inline asm complications. */
#if defined (TARGET_OS_WINDOWS) && !defined (TARGET_CPU_X86)
#undef ENABLE_CPUID
#endif

#include <sys/stat.h>

#if defined (TARGET_OS_WINDOWS) && (defined (TARGET_COMPILER_VC) || defined (TARGET_COMPILER_ICC))
#if TARGET_CPU_BITS == 32
#define ENABLE_SYMBOL_ENGINE
#else
#undef ENABLE_SYMBOL_ENGINE
#undef ENABLE_BACKTRACE
#endif
#if _MSC_VER > 1200 && _MSC_VER < 1400
#pragma warning ( disable : 4345 4100 4800 )
#endif
#if _MSC_VER <= 1200
#undef DETECT_MEMORY_LEAKS
#endif
#if defined (_DEBUG)
#undef ENABLE_CRASHREPORTS
#endif
#if defined (DETECT_MEMORY_LEAKS)
#define _CRTDBG_MAP_ALLOC
#endif
#if defined (ENABLE_CRASHREPORTS)
#include "exception_handler.h"
#endif
#include <io.h>
#include <fcntl.h>
#include <winsock2.h>
#include <mstcpip.h>
#include <windows.h>
#include <crtdbg.h>
#if defined (ENABLE_SYMBOL_ENGINE)
#include <dbghelp.h>
#pragma comment (lib, "dbghelp.lib")
#endif
#include <process.h>
#include <shlobj.h>
#define stat _stat
#define strcasecmp stricmp
#else
#undef ENABLE_SYMBOL_ENGINE
#undef ENABLE_CRASHREPORTS
#endif

#if defined (TARGET_OS_NDSFIRMWARE)
#define ANSI_COLOUR
#ifdef __cplusplus
#include <cxxabi.h>
#endif
#include <nds.h>
#include <fat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#endif

#if defined (TARGET_OS_LINUX) || defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
#define ANSI_COLOUR
#ifdef __cplusplus
#include <cxxabi.h>
#endif
#include <pthread.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <ctype.h>
#endif

#if defined (TARGET_COMPILER_CYGWIN)
#undef ENABLE_BACKTRACE
#undef HAS_FPOS64
#endif

#if defined (TARGET_OS_LINUX) && defined (ENABLE_BACKTRACE)
#include <execinfo.h>
#endif

#include <string.h>
#include <assert.h>
#include <math.h>
#if !defined (TARGET_OS_NDSFIRMWARE)
#include <memory.h>
#else
#include <malloc.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if _MSC_VER < 1300 && defined (TARGET_COMPILER_VC)
#include <xstring>
typedef long intptr_t;
#endif
#ifdef __cplusplus
#include <exception>
#include <functional>
#include <queue>
#include <vector>
#include <iostream>
#include <iomanip>
#include <istream>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#endif

__inline char *cc_strdup(const char *x)
{
	if (!x) return NULL;

	char *dup = (char *)malloc(strlen(x) + 1);
	if (!dup) return NULL;

	return strcpy(dup, x);
}

#define cc_min(x, y) ((x < y) ? x : y)
#define cc_max(x, y) ((x > y) ? x : y)

#if defined (TARGET_COMPILER_VC)
#if _MSC_VER < 1400
extern "C" int __cdecl _fseeki64(FILE *, __int64, int);
extern "C" __int64 __cdecl _ftelli64(FILE *);
#endif
#endif

#if defined (TARGET_OS_WINDOWS)
#if defined (DETECT_MEMORY_LEAKS)
#ifndef _DBG_NEW
inline void *__operator_new(size_t __n)
{
	return ::operator  new(__n, _NORMAL_BLOCK, __FILE__, __LINE__);
}

inline void *_cdecl operator
new(size_t __n, const char *__fname, int __line)
{
	return ::operator  new(__n, _NORMAL_BLOCK, __fname, __line);
}

inline void _cdecl operator
delete(void *__p, const char *, int)
{
	::operator  delete(__p);
}

#define _DBG_NEW new (__FILE__, __LINE__)
#define new _DBG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;


#endif
#endif

/*! \brief The application entry point. */
extern int RunApplication(int argc, char * *argv);
/*! \brief An internal initialization function for the program. */
int CrissCrossInitialize(int argc, char * *argv);

/* Namespace Definitions */
/* Primarily here for centralised documentation */

/*! \brief The CrissCross main namespace. */
/*!
 *  All CrissCross classes and functions are kept in this namespace.
 */
namespace CrissCross
{
	/*! \brief The cryptographic routine namespace. */
	/*!
	 *      Contains a variety of hashing algorithms for general use.
	 */
	namespace Crypto
	{
	}

	/*! \brief The data structure namespace. */
	/*!
	 *  Contains basic data structures to assist in sorting and searching
	 *  for data.
	 */
	namespace Data
	{
	}

	/*! \brief The debug namespace. */
	/*!
	 *  Contains classes and functions designed to assist programmers
	 *  in debugging their software.
	 */
	namespace Debug
	{
	}

	/*! \brief The input/output namespace. */
	/*!
	 *  Contains classes for console and file I/O.
	 */
	namespace IO
	{
	}

	/*! \brief The network input/output namespace. */
	/*!
	 *  Contains classes which enable programmers to use TCP, UDP,
	 *  and other network protocols to transfer data.
	 */
	namespace Network
	{
	}

	/*! \brief The system call namespace. */
	/*!
	 *  Contains platform-specific API call wrappers, which allow the programmer
	 *  to write code which is cross-platform and functions identically on all
	 *  platforms.
	 */
	namespace System
	{
	}

	/*! \brief The library version namespace. */
	/*!
	 *  Contains some simple functions for identifying the compiled CrissCross
	 *  library version.
	 */
	namespace Version
	{
	}
}

#include <crisscross/datatypes.h>

#endif
