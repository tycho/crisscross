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

#ifndef __included_cc_universal_include_h
#define __included_cc_universal_include_h

#ifdef _MSC_VER
#pragma warning (disable:4530)
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

#define CC_LIB_URL                 "http://www.uplinklabs.net/crisscross/"
#define CC_LIB_COPYRIGHT           "(c) 2006-2010 by Uplink Laboratories. Licensed under the New BSD License."
#define CC_LIB_NDS_COPYRIGHT       "(c) by Uplink Laboratories.\nLicensed under the BSD License."

/* #define DISABLE_DEPRECATED_CODE */ /* This will be enabled by default in a future release */

/* NOTE: By disabling this line, you will not be in compliance with the New BSD License. */
/*      If you disable this line, you must display the copyright notice in the program */
/*      elsewhere. */
#define ENABLE_CREDITS

/* Linux backtrace() */
#define ENABLE_BACKTRACE

#define ENABLE_PROFILER

/* Case sensitive Compare() function */
/* #    define CASE_SENSITIVE_COMPARE */

/* ============================================================================ */
/* Dont edit anything below this line */
/* ============================================================================ */

#include <crisscross/platform_detect.h>

#include <crisscross/cc_attr.h>

/* ----------------------- */
/* RESULTANT CONFIGURATION */
/* ----------------------- */

#if defined (TARGET_OS_WINDOWS)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#endif
#ifndef _WIN32_IE
#define _WIN32_IE   _WIN32_IE_IE70
#endif
#endif

#if !defined (_CONSOLE)
#undef  _WINDOWS
#define _CONSOLE
#endif
#define _CC_ENABLE_OPENGL

#if defined (_OPENMP)
#include <omp.h>
#endif

#ifdef TARGET_COMPILER_VC
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
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

#if defined (TARGET_OS_WINDOWS) && (defined (TARGET_COMPILER_VC) || defined (TARGET_COMPILER_ICC))
#define ENABLE_SYMBOL_ENGINE
#ifdef _MSC_VER
#if _MSC_VER > 1200 && _MSC_VER < 1400
#pragma warning ( disable : 4345 4100 4800 )
#endif
#endif
#else
#undef ENABLE_SYMBOL_ENGINE
#endif

#if defined (TARGET_COMPILER_CYGWIN)
#undef ENABLE_BACKTRACE
#undef HAS_FPOS64
#endif

#if defined (TARGET_COMPILER_BORLAND)
#undef ENABLE_BACKTRACE
#endif

#include <stddef.h>

#include <crisscross/compare.h>

#define cc_min(x, y) (CrissCross::Data::Compare(x, y) < 0 ? x : y)
#define cc_max(x, y) (CrissCross::Data::Compare(x, y) > 0 ? x : y)

#if defined (TARGET_COMPILER_VC)
#if _MSC_VER < 1400
extern "C" int __cdecl _fseeki64(FILE *, __int64, int);
extern "C" __int64 __cdecl _ftelli64(FILE *);
#endif
#endif


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

	/*! \brief The filesystem namespace. */
	/*!
	 *  Contains classes and functions for filesystem interaction.
	 */
	namespace Filesystem
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
