/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2021 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_cc_attr_h
#define __included_cc_cc_attr_h

#if defined(__GNUC__) && __GNUC__ >= 3

#ifndef cc_pure
#define cc_pure         __attribute__ ((pure))
#endif

#ifndef cc_forceinline
#define cc_forceinline  __attribute__ ((always_inline))
#endif

#ifndef cc_noreturn
#define cc_noreturn     __attribute__ ((noreturn))
#endif

#ifndef cc_malloc
#define cc_malloc       __attribute__ ((malloc))
#endif

#ifndef cc_must_check
#define cc_must_check   __attribute__ ((warn_unused_result))
#endif

#ifndef cc_deprecated
#define cc_deprecated   __attribute__ ((deprecated))
#endif

#ifndef cc_used
#define cc_used         __attribute__ ((used))
#endif

#ifndef cc_unused_param
#define cc_unused_param __attribute__ ((unused))
#endif

#ifndef cc_packed
#define cc_packed       __attribute__ ((packed))
#endif

#ifndef cc_likely
#define cc_likely(x)      __builtin_expect(!!(x), 1)
#endif

#ifndef cc_uncc_likely
#define cc_uncc_likely(x)    __builtin_expect(!!(x), 0)
#endif

#define cc_assume(x)    do { if (!(x)) __builtin_unreachable(); } while (0)

#elif defined(_MSC_VER)

#define cc_assume(x)      __assume(x)

#endif

#ifndef cc_pure
#define cc_pure
#endif

#ifndef __const
#define __const
#endif

#ifndef cc_forceinline
#define cc_forceinline  inline
#endif

#ifndef cc_noreturn
#define cc_noreturn
#endif

#ifndef cc_malloc
#define cc_malloc
#endif

#ifndef cc_must_check
#define cc_must_check
#endif

#ifndef cc_deprecated
#define cc_deprecated
#endif

#ifndef cc_used
#define cc_used
#endif

#ifndef cc_unused_param
#define cc_unused_param
#endif

#ifndef cc_packed
#define cc_packed
#endif

#ifndef cc_likely
#define cc_likely(x)      (x)
#endif

#ifndef cc_unlikely
#define cc_unlikely(x)    (x)
#endif

#ifndef cc_assume
#define cc_assume(x)
#endif

#if defined (TARGET_COMPILER_GCC)
#if (__GNUC__ >= 4)
#define _CC_DEPRECATE_TEXT(_Text)  __attribute__((__deprecated__))
#else
#define _CC_DEPRECATE_TEXT(_Text)
#endif
#elif defined (TARGET_COMPILER_VC)
#if _MSC_FULL_VER >= 140050320
#define _CC_DEPRECATE_TEXT(_Text) __declspec(deprecated(_Text))
#elif _MSC_VER > 1200
#define _CC_DEPRECATE_TEXT(_Text) __declspec(deprecated)
#else
#define _CC_DEPRECATE_TEXT(_Text)
#endif
#else
#define _CC_DEPRECATE_TEXT(_Text)
#endif

#if !defined (_CC_NO_DEPRECATE)
#ifndef _CC_SLOW_NO_DEPRECATE
#define _CC_DEPRECATE_SLOW(x)                    _CC_DEPRECATE_TEXT("This function or class is known to be slow and should not be used in production.")
#else
#define _CC_DEPRECATE_SLOW(x)
#endif
#ifndef _CC_ONLY_DEPRECATE_SLOW
#define _CC_DEPRECATE_CLASS(_Replacement)        _CC_DEPRECATE_TEXT("This class is deprecated and may be removed from future versions. Consider using '" # _Replacement "' instead. Be sure to check the CrissCross documentation.")
#define _CC_DEPRECATE_FUNCTION(_Replacement)     _CC_DEPRECATE_TEXT("This function is deprecated and may be removed from future versions. Consider using '" # _Replacement "' instead. Be sure to check the CrissCross documentation.")
#define _CC_DEPRECATE_FUNCTION_N                 _CC_DEPRECATE_TEXT("This function is deprecated and may be removed from future versions. Be sure to check the CrissCross documentation.")
#else
#define _CC_DEPRECATE_CLASS(_Replacement)
#define _CC_DEPRECATE_FUNCTION(_Replacement)
#define _CC_DEPRECATE_FUNCTION_N
#endif
#else
#define _CC_DEPRECATE_SLOW(x)
#define _CC_DEPRECATE_CLASS(_Replacement)
#define _CC_DEPRECATE_FUNCTION(_Replacement)
#define _CC_DEPRECATE_FUNCTION_N
#endif

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#define _CC_WARN_UNUSED_RESULT [[nodiscard]]
#else
#define _CC_WARN_UNUSED_RESULT
#endif

#endif
