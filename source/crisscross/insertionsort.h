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

#ifndef __included_cc_insertionsort_h
#define __included_cc_insertionsort_h

#ifdef ENABLE_SORTS

#include <crisscross/sort.h>

namespace CrissCross
{
	namespace Data
	{
#if !defined(DISABLE_DEPRECATED_CODE)
		/*! \brief A InsertionSort implementation for sorting arrays. */
		/*!
		 * \deprecated This is an extremely slow sorting method. It is provided for educational purposes ONLY.
		 */
		template <class T>
		class InsertionSort : public Sorter<T>
		{
			public:
				/*! \brief Sorts an array using the InsertionSort method. */
				/*!
				 * \param _array The array to sort.
				 * \param _size The size of the array to sort.
				 * \return 0 on success.
				 * \sa HeapSort ShellSort CombSort
				 */
				_CC_DEPRECATE_SLOW("SEVERE") int Sort(T *_array, size_t _size)
				{
					for (long i = 1; i < (long)_size; i++) {
						long j = i;
						T    b = _array[i];
						while ((j > 0) && (Compare(_array[j - 1], b) > 0)) {
							_array[j] = _array[j - 1];
							j--;
						}
						_array[j] = b;
					}

					return 0;
				};
		};
#endif
	}
}

#endif

#endif
