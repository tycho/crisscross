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

#ifndef __included_cc_sort_h
#define __included_cc_sort_h

#ifdef ENABLE_SORTS

#include <crisscross/deprecate.h>
#include <crisscross/compare.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief Sorting abstract class. */
		template <class T>
		class Sorter
		{
			public:
				/*! \brief The constructor. */
				Sorter()
				{
				};

				/*! \brief The destructor. */
				virtual ~Sorter()
				{
				};

				/*! \brief The abstract sort function. */
				/*!
				 *                  Does nothing, and cannot be called (abstract).
				 * \param _array The array to sort.
				 * \param _size The size of the array to sort.
				 */
				virtual int Sort(T *_array, size_t _size) = 0;

				/*! \brief The swap function. */
				/*!
				 *                  Will swap two items in an array.
				 * \param _array The array to swap items in.
				 * \param _first The first item to swap.
				 * \param _second The second item to swap.
				 */
				virtual void Swap(T *_array, size_t _first, size_t _second)
				{
					T old = _array[_first];
					_array[_first] = _array[_second];
					_array[_second] = old;
				};
		};
	}
}

#include <crisscross/bubblesort.h>
#include <crisscross/combsort.h>
#include <crisscross/heapsort.h>
#include <crisscross/insertionsort.h>
#include <crisscross/quicksort.h>
#include <crisscross/shellsort.h>

#define FastestSort                ShellSort
#define MemoryEfficientSort        HeapSort

#endif

#endif
