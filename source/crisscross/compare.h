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

#ifndef __included_cc_compare_h
#define __included_cc_compare_h

#include <crisscross/debug.h>

#ifdef TARGET_OS_WINDOWS
#define strcasecmp stricmp
#endif

namespace CrissCross
{
	namespace Data
	{
		/*! \brief Function for generic data comparison. */
		/*!
		 *  Can compare any class with the less-than and greater-than operators, as well as C-style strings.
		 * \param _first The item to compare to _second. Must have comparison operators > and < implemented.
		 * \param _second The item to compare to _first. Must have comparison operators > and < implemented.
		 * \return 0 if the items are equal, -1 if _first is less than _second, and 1 if _first is greater than _second.
		 */
		template <class T>
		__inline int Compare(T const &_first, T const &_second)
		{
			if (_first < _second)
				return -1;
			else if (_first > _second)
				return 1;
			else
				return 0;
		}

		/*! \brief C-style string CrissCross::Data::Compare function. */
		template <>
		__inline int Compare<char *> (char * const &_first, char * const &_second)
		{
			CoreAssert(_first); CoreAssert(_second);
#ifdef CASE_SENSITIVE_COMPARE
			int ret = ::strcmp(_first, _second);
#else
			int ret = ::strcasecmp(_first, _second);
#endif
			if (ret < 0) return -1;
			else if (ret > 0) return 1;
			else return 0;
		}

		/*! \brief C-style string CrissCross::Data::Compare function. */
		template <>
		__inline int Compare<const char *> (const char * const &_first, const char * const &_second)
		{
			CoreAssert(_first); CoreAssert(_second);
#ifdef CASE_SENSITIVE_COMPARE
			int ret = ::strcmp(_first, _second);
#else
			int ret = ::strcasecmp(_first, _second);
#endif
			if (ret < 0) return -1;
			else if (ret > 0) return 1;
			else return 0;
		}
	}
}

#endif
