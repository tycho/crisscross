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

#ifndef __included_cc_compare_h
#define __included_cc_compare_h

#include <crisscross/debug.h>

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
		template <> int Compare<char *> (char * const &_first, char * const &_second);

		/*! \brief C-style string CrissCross::Data::Compare function. */
		template <>	int Compare<const char *> (const char * const &_first, const char * const &_second);

		/*! \brief An STL-compatible comparator class which makes use of Compare */
		template <class T>
		class LessThanComparator
		{
		public:
			bool operator()(T const &_first, T const &_second) const
			{
				return Compare<T>(_first, _second) < 0;
			}
		};

		/*! \brief An STL-compatible comparator class which makes use of Compare */
		template <class T>
		class GreaterThanComparator
		{
		public:
			bool operator()(T const &_first, T const &_second) const
			{
				return Compare<T>(_first, _second) > 0;
			}
		};
	}
}

#endif
