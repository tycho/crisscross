/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2018 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_nasty_cast_h
#define __included_cc_nasty_cast_h

#include <crisscross/platform_detect.h>

#ifndef TARGET_COMPILER_BORLAND
template <typename TO, typename FROM> TO nasty_cast(FROM f) {
	union {
		FROM f; TO t;
	} u; u.f = f; return u.t;
}
#endif

#endif
