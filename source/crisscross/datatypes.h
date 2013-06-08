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

#ifndef __included_cc_datatypes_h
#define __included_cc_datatypes_h

#include <crisscross/pstdint.h>

#ifdef TARGET_OS_WINDOWS
typedef int64_t fpos64_t;
#elif defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || \
        defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
typedef fpos_t fpos64_t;
#endif

#endif
