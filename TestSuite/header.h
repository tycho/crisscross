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

#ifndef __included_header_h
#define __included_header_h

#define CC_ENABLE_STACK_WALKER

#include <crisscross/universal_include.h>

#ifdef TARGET_COMPILER_VC
#  define VC_EXTRALEAN
#  define WIN32_LEAN_AND_MEAN
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

#include <crisscross/crisscross.h>

#include "testutils.h"

/* #define HIGH_INTENSITY */
/* #define FILE_CHECKSUM */

#ifdef HIGH_INTENSITY
#  define TREE_ITEMS 1024
#  define SORT_ITEMS 10000
#else
#  define TREE_ITEMS 128
#  define SORT_ITEMS 100
#endif

#endif
