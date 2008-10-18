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

#ifndef __included_cc_bubblesort_h
#define __included_cc_bubblesort_h

#ifdef ENABLE_SORTS

#include <crisscross/sort.h>

namespace CrissCross
{
	namespace Data
	{
#if !defined(DISABLE_DEPRECATED_CODE)
		/*! \brief A BubbleSort implementation for sorting arrays. */
		/*!
		 * \deprecated This is an extremely slow sorting method. It is provided for educational purposes ONLY.
		 */
		template <class T>
		class BubbleSort : public Sorter<T>
		{
			public:
				/*! \brief Sorts an array using the BubbleSort method. */
				/*!
				 * \param _array The array to sort.
				 * \param _size The size of the array to sort.
				 * \return 0 on success.
				 * \sa HeapSort ShellSort CombSort
				 */
				_CC_DEPRECATE_SLOW("EXTREME") int Sort(T *_array, size_t _size)
				{
					for (size_t i = _size - 1; i >= 0; i--) {
						bool flipped = false;
						for (size_t j = 0; j < i; j++) {
							if (Compare(_array[j], _array[j + 1]) > 0) {
								Swap(_array, j, j + 1);
								flipped = true;
							}
						}

						if (!flipped || i == 0) {
							break;
						}
					}

					return 0;
				};
		};
#endif
	}
}

#endif

#endif
