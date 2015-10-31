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

#ifndef __included_cc_cc_attr_h
#define __included_cc_cc_attr_h

#if defined(__GNUC__) && __GNUC__ >= 3

#ifndef __pure
#define __pure         __attribute__ ((pure))
#endif

#ifndef __forceinline
#define __forceinline  __attribute__ ((always_inline))
#endif

#ifndef __noreturn
#define __noreturn     __attribute__ ((noreturn))
#endif

#ifndef __malloc
#define __malloc       __attribute__ ((malloc))
#endif

#ifndef __must_check
#define __must_check   __attribute__ ((warn_unused_result))
#endif

#ifndef __deprecated
#define __deprecated   __attribute__ ((deprecated))
#endif

#ifndef __used
#define __used         __attribute__ ((used))
#endif

#ifndef __unused_param
#define __unused_param __attribute__ ((unused))
#endif

#ifndef __packed
#define __packed       __attribute__ ((packed))
#endif

#ifndef likely
#define likely(x)      __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x)    __builtin_expect(!!(x), 0)
#endif

#define assume(x)    do { if (!(x)) __builtin_unreachable(); } while (0)

#elif defined(_MSC_VER)

#define assume(x)      __assume(x)

#endif

#ifndef __pure
#define __pure
#endif

#ifndef __const
#define __const
#endif

#ifndef __forceinline
#define __forceinline  inline
#endif

#ifndef __noreturn
#define __noreturn
#endif

#ifndef __malloc
#define __malloc
#endif

#ifndef __must_check
#define __must_check
#endif

#ifndef __deprecated
#define __deprecated
#endif

#ifndef __used
#define __used
#endif

#ifndef __unused_param
#define __unused_param
#endif

#ifndef __packed
#define __packed
#endif

#ifndef likely
#define likely(x)      (x)
#endif

#ifndef unlikely
#define unlikely(x)    (x)
#endif

#ifndef assume
#define assume(x)
#endif

#endif
