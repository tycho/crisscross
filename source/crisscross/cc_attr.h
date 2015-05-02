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

#if __GNUC__ >= 3

#ifndef __pure
#define __pure         __attribute__ ((pure))
#endif

/*
  GCC 3.x doesn't like to inline some of the things that
  we use __forceinline on.
 */
#if __GNUC__ > 3
#ifndef __forceinline
#define __forceinline  __attribute__ ((always_inline))
#endif
#else
#define __forceinline  inline
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

#else

#define __pure
#define __const
#define __forceinline  inline
#define __noreturn
#define __malloc
#define __must_check
#define __deprecated
#define __used
#define __unused_param
#define __packed
#define likely(x)      (x)
#define unlikely(x)    (x)

#endif

#endif
