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

#ifndef __included_cc_combsort_h
#define __included_cc_combsort_h

#ifdef ENABLE_SORTS

#include <crisscross/sort.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A CombSort implementation for sorting arrays. */
		/*!
		 * CombSort isn't intended for mainstream use, but it's technically very
		 * interesting. The main reason it's interesting is that it was created
		 * by making some relatively small changes to BubbleSort. These tiny
		 * changes made a new algorithm that still rivals more mainstream
		 * algorithms like HeapSort, ShellSort, and others.
		 */
		template <class T>
		class CombSort : public Sorter<T>
		{
			private:
				size_t NewGap(size_t gap)
				{
					gap = (gap * 10) / 13;
					if (gap == 9 || gap == 10)
						gap = 11;

					if (gap < 1)
						gap = 1;

					return gap;
				};

			public:
				/*! \brief Sorts an array using the CombSort method. */
				/*!
				 * \param _array The array to sort.
				 * \param _size The size of the array to sort.
				 * \return 0 on success.
				 */
				int Sort(T *_array, size_t _size)
				{
					size_t gap = _size;
					for (;;) {
						gap = NewGap(gap);
						bool swapped = false;
						for (size_t i = 0; i < _size - gap; i++) {
							size_t j = i + gap;
							if (Compare(_array[i], _array[j]) > 0) {
								Swap(_array, i, j);
								swapped = true;
							}
						}

						if (gap == 1 && !swapped)
							break;
					}

					return 0;
				};
		};
	}
}

#endif

#endif
