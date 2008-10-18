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

#ifndef __included_cc_shellsort_h
#define __included_cc_shellsort_h

#ifdef ENABLE_SORTS

#include <crisscross/sort.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A ShellSort implementation for sorting arrays. */
		template <class T>
		class ShellSort : public Sorter<T>
		{
			public:
				/*! \brief Sorts an array using the ShellSort method. */
				/*!
				 * \param _array The array to sort.
				 * \param _size The size of the array to sort.
				 * \return 0 on success.
				 */
				int Sort(T *_array, size_t _size)
				{
					size_t h = 1;
					while ((h * 3 + 1) < _size) {
						h = 3 * h + 1;
					}
					while (h > 0) {
						for (size_t i = h - 1; i < _size; i++) {
							T      B = _array[i];
							size_t j = i;

							for (j = i; (j >= h) && (Compare(_array[j - h], B) > 0); j -= h) {
								_array[j] = _array[j - h];
							}

							_array[j] = B;
						}

						h = h / 3;
					}
					return 0;
				};
		};
	}
}

#endif

#endif
