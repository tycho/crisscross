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

#ifndef __included_cc_quicksort_h
#define __included_cc_quicksort_h

#ifdef ENABLE_SORTS

#include <crisscross/deprecate.h>
#include <crisscross/sort.h>

namespace CrissCross
{
	namespace Data
	{
#if !defined(DISABLE_DEPRECATED_CODE)
		/*! \brief A QuickSort implementation for sorting arrays. */
		/*!
		 * \deprecated This is a slow sorting method (worst case would be with sorting a reverse-sorted dataset). It is provided for educational purposes ONLY.
		 */
		template <class T>
		class QuickSort : public Sorter<T>
		{
			private:
				int InternalSort(T *_array, size_t _beginning, size_t _end)
				{
					if (_end > _beginning + 1) {
						const T &piv = _array[_beginning];
						size_t   l = _beginning + 1, r = _end;
						while (l < r) {
							if (Compare(_array[l], piv) <= 0)
								l++;
							else
								Swap(_array, l, --r);
						}
						Swap(_array, --l, _beginning);
						InternalSort(_array, _beginning, l);
						InternalSort(_array, r, _end);
					}

					return 0;
				};

			public:

				/*! \brief Sorts an array using the QuickSort method. */
				/*!
				 * \param _array The array to sort.
				 * \param _size The size of the array to sort.
				 * \return 0 on success.
				 * \sa HeapSort ShellSort CombSort
				 */
				_CC_DEPRECATE_SLOW("MODERATE") int Sort(T *_array, size_t _size)
				{
					return InternalSort(_array, 0, _size);
				};
		};
#endif
	}
}

#endif

#endif
