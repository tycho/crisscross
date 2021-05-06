/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2021 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_darray_h
#error "This file shouldn't be compiled directly."
#endif

#include <crisscross/debug.h>

#include <crisscross/darray.h>
#include <crisscross/dstack.h>

namespace CrissCross
{
	namespace Data
	{
		template <class T>
		DArray <T>::DArray()
		{
			static_assert(std::is_trivially_copyable<T>::value);
			m_stepSize = -1;
			m_numUsed = m_arraySize = 0;
			m_array = NULL;
		}

		template <class T>
		DArray <T>::DArray(T *_array, size_t _indices)
		{
			m_array = new T[_indices];
			memcpy(m_array, _array, _indices * sizeof(T));

			m_shadow.resize(_indices, false);

			m_numUsed = 0;
			for (size_t i = 0; i < _indices; i++) {
				m_shadow[i] = (m_array[i] != NULL) ? true : false;
				if (m_shadow[i]) {
					m_numUsed++;
				}
			}

			m_arraySize = _indices;
			m_stepSize = -1;
		}

		template <class T>
		DArray <T>::DArray(DArray<T> const &_array)
		{
			m_array = new T[_array.m_arraySize];
			memcpy(m_array, _array.m_array, _array.m_arraySize * sizeof(T));

			m_shadow.resize(_array.m_arraySize);

			m_numUsed = 0;
			for (size_t i = 0; i < _array.m_arraySize; i++)	{
				m_shadow[i] = (m_array[i] != NULL) ? true : false;
				if (m_shadow[i]) {
					m_numUsed++;
				}
			}

			m_arraySize = _array.m_arraySize;
			m_stepSize = _array.m_stepSize;
		}

		template <class T>
		DArray <T>::DArray(int _newStepSize)
		{
			if (_newStepSize < 1)
				m_stepSize = -1;
			else
				m_stepSize = _newStepSize;

			m_numUsed = m_arraySize = 0;
			m_array = NULL;
		}

		template <class T>
		DArray <T>::~DArray()
		{
			empty();
		}

		template <class T>
		void DArray <T>::compact()
		{
			constexpr size_t pageSizeInElements = std::max((size_t)(4096u / sizeof(T)), (size_t)32u);

			/* Only resize if it would possibly be beneficial. */
			if ((m_arraySize * sizeof(T)) <= 4096)
				return;
			if (m_numUsed > m_arraySize / 2)
				return;

			auto found = std::find(std::rbegin(m_shadow), std::rend(m_shadow), 1);
			size_t idx = std::distance(found, m_shadow.rend());

			if (idx > m_arraySize)
				return;

			if ((m_arraySize - idx) < pageSizeInElements * 2)
				return;
			
			/* Only shrink if the array is significantly under-used */
			float inUse = (float)idx / (float)m_arraySize;
			if (inUse < 0.33f) {
				/* Round up to next page boundary. */
				idx = ((idx / pageSizeInElements) + 1) * pageSizeInElements;
				setSize(idx);
			}
		}

		template <class T>
		void DArray <T>::recount()
		{
			m_numUsed = 0;
			for (size_t i = 0; i < m_arraySize; i++)
				if (m_shadow[i])
					m_numUsed++;
		}

		template <class T>
		void DArray <T>::reserve(size_t newsize)
		{
			if (newsize > m_arraySize) {
				setSize(newsize);
				m_shadow.clear();
				m_shadow.resize(newsize, true);
			}
		}

		template <class T>
		void DArray <T>::setMinimumSize(size_t newsize)
		{
			if (m_arraySize < newsize)
				setSize(newsize);
		}

		template <class T>
		void DArray <T>::setSize(size_t newsize)
		{
			m_shadow.resize(newsize, false);
			if (newsize > m_arraySize) {
				size_t oldarraysize = m_arraySize;

				m_arraySize = newsize;
				T *temparray = new T[m_arraySize];

				if (m_array) {
					for(size_t i = 0; i < oldarraysize; i++)
					{
						temparray[i] = m_array[i];
					}
				}

				delete [] m_array;

				m_array = temparray;
			} else if (newsize < m_arraySize) {
				m_arraySize = newsize;
				T *temparray = new T[m_arraySize];

				if (m_array) {
					for(size_t i = 0; i < m_arraySize; i++)
					{
						temparray[i] = m_array[i];
					}
				}

				/* We may have lost more than one node. It's worth rebuilding over. */
				recount();

				delete [] m_array;

				m_array = temparray;
			} else if (newsize == m_arraySize) {
				/* Do nothing */
			}
		}

		template <class T>
		void DArray <T>::grow()
		{
			/* Most of the data types we store in DArrays will the system
			 * pointer size. Try to keep our array approximately a multiple of
			 * a 4K page
			 */
			const size_t growthHeuristic = 4096 / sizeof(T);

			if (m_stepSize == -1) {
				if (m_arraySize == 0) {
					setSize(8);
				} else if (m_arraySize <= growthHeuristic) {
					setSize(m_arraySize * 2);
				} else {
					setSize(m_arraySize + growthHeuristic);
				}
			} else {
				/* Increase array size by fixed amount */
				setSize(m_arraySize + m_stepSize);
			}
		}

		template <class T>
		void DArray <T>::setStepSize(int _stepSize)
		{
			m_stepSize = _stepSize;
		}

		template <class T>
		void DArray <T>::setStepDouble()
		{
			m_stepSize = -1;
		}

		template <class T>
		size_t DArray <T>::insert(T const & newdata)
		{
			size_t freeslot = getNextFree();
			m_array[freeslot] = newdata;
			m_shadow[freeslot] = true;
			m_numUsed++;
			return freeslot;
		}

		template <class T>
		void DArray <T>::insert(T const & newdata, size_t index)
		{
			while (index >= m_arraySize)
				grow();
			m_array[index] = newdata;
			if (!m_shadow[index]) {
				m_shadow[index] = true;
				m_numUsed++;
			}
		}

		template <class T>
		void DArray <T>::empty(bool _freeMemory)
		{
			m_shadow.clear();

			m_numUsed = 0;

			if (_freeMemory) {
				m_shadow.resize(0);
				delete [] m_array;
				m_array = NULL;
				m_arraySize = 0;
			} else {
				m_shadow.resize( 0, false );
				m_shadow.resize( m_arraySize, false );
			}
		}

		template <class T>
		size_t DArray <T>::getNextFree()
		{
			if (!m_array)
				grow();

			size_t freeslot = (size_t)-1;

			while (freeslot == (size_t)-1) {
				auto found = std::find(std::begin(m_shadow), std::end(m_shadow), false);
				if (found != std::end(m_shadow))
					freeslot = std::distance(std::begin(m_shadow), found);
				else
					grow();
			}

			return freeslot;
		}

		template <class T>
		T DArray <T>::get(size_t index) const
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);

			return m_array[index];
		}

		template <class T>
		T & DArray <T>::operator [](size_t index)
		{
			CoreAssert(index < m_arraySize);
			if (!m_shadow[index]) {
				m_shadow[index] = true;
				m_numUsed++;
			}
			return m_array[index];
		}

		template <class T>
		const T &DArray <T>::operator [](size_t index) const
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);

			return m_array[index];
		}

		template <class T>
		size_t DArray<T>::mem_usage() const
		{
			size_t ret = sizeof(*this);
			ret += m_arraySize * sizeof(T);
			return ret;
		}

		template <class T>
		void DArray <T>::remove(size_t index)
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);

			m_numUsed--;

			m_shadow[index] = false;
		}

		template <class T>
		size_t DArray <T>::find(T const & newdata)
		{
			for (size_t a = 0; a < m_arraySize; ++a)
				if (m_shadow[a])
					if (Compare(m_array[a], newdata) == 0)
						return a;

			return -1;
		}

		template <class T>
		int DArray <T>::sort(Sorter<T> *_sortMethod)
		{
			size_t idx = 0;
			int ret;

			if (m_numUsed < 2)
				return 0;

			T *temp_array = new T[m_numUsed];

			memset(temp_array, 0, m_numUsed * sizeof(T));

			for (size_t i = 0; i < m_arraySize; i++) {
				if (valid(i)) {
					CoreAssert(idx < m_numUsed);
					temp_array[idx++] = m_array[i];
				}
			}

			ret = _sortMethod->Sort(temp_array, m_numUsed);

			m_shadow.clear();
			m_shadow.resize(0);
			m_shadow.resize(m_numUsed, true);

			delete [] m_array;
			m_array = temp_array;

			m_arraySize = m_numUsed;

			recount();

			return ret;
		}

		template <class T>
		int DArray <T>::sort(Sorter<T> &_sortMethod)
		{
			return sort(&_sortMethod);
		}

		template <class T>
		void DArray<T>::flush()
		{
			static_assert(std::is_pointer<T>::value, "attempting to delete a non-pointer type");
			for (size_t i = 0; i < m_arraySize; ++i) {
				if (valid(i)) {
					delete m_array[i];
				}
			}
			empty();
		}

		template <class T>
		void DArray<T>::flushArray()
		{
			static_assert(std::is_pointer<T>::value, "attempting to delete a non-pointer type");
			for (size_t i = 0; i < m_arraySize; ++i) {
				if (valid(i)) {
					delete [] m_array[i];
				}
			}
			empty();
		}


		/* BELOW ARE DEPRECATED FUNCTIONS */

#if !defined (DISABLE_DEPRECATED_CODE)
		template <class T>
		void DArray<T>::EmptyAndDelete()
		{
			static_assert(std::is_pointer<T>::value, "attempting to delete a non-pointer type");
			for (size_t i = 0; i < m_arraySize; ++i) {
				if (valid(i)) {
					delete m_array[i];
				}
			}

			empty();
		}

		template <class T>
		void DArray<T>::EmptyAndDeleteArray()
		{
			static_assert(std::is_pointer<T>::value, "attempting to delete a non-pointer type");
			for (size_t i = 0; i < m_arraySize; ++i) {
				if (valid(i)) {
					delete [] m_array[i];
				}
			}
			empty();
		}

		template <class T>
		void DArray<T>::ChangeData(T const & _rec, size_t index)
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);
			m_array[index] = _rec;
		}
#endif
	}
}
