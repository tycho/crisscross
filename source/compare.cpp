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

#include <crisscross/compare.h>

#ifdef TARGET_OS_WINDOWS
#define strcasecmp stricmp
#endif

namespace CrissCross
{
	namespace Data
	{
		/*! \brief C-style string CrissCross::Data::Compare function. */
		template <>
		int Compare<char *> (char * const &_first, char * const &_second)
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
		int Compare<const char *> (const char * const &_first, const char * const &_second)
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
