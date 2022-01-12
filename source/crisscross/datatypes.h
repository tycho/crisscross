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

#ifndef __included_cc_datatypes_h
#define __included_cc_datatypes_h

#include <stdint.h>
#ifdef TARGET_OS_MACOSX
#include <stdio.h>
#endif

#ifdef TARGET_OS_WINDOWS
typedef int64_t fpos64_t;
#elif defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || \
        defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
typedef fpos_t fpos64_t;
#endif

#endif
