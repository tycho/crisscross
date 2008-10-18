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

#ifndef __included_cc_deprecate_h
#define __included_cc_deprecate_h

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
#define _CC_DEPRECATE_SLOW(x)                    _CC_DEPRECATE_TEXT("When used, this function or class can result in a " x " slowdown in execution speed.")
#else
#define _CC_DEPRECATE_SLOW(x)
#endif
#ifndef _CC_ONLY_DEPRECATE_SLOW
#define _CC_DEPRECATE_CLASS(_Replacement)        _CC_DEPRECATE_TEXT("This class is deprecated and may be removed from future versions. Consider using '" # _Replacement "' instead.")
#define _CC_DEPRECATE_FUNCTION(_Replacement)     _CC_DEPRECATE_TEXT("This function is deprecated and may be removed from future versions. Consider using '" # _Replacement "' instead.")
#define _CC_DEPRECATE_FUNCTION_N                 _CC_DEPRECATE_TEXT("This function is deprecated and may be removed from future versions.")
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
#endif
