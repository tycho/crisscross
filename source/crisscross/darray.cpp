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

#include <memory>

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
			m_stepSize = -1;
			m_numUsed = m_arraySize = m_nextInsertPos = 0;
			m_array = NULL;
		}

		template <class T>
		DArray <T>::DArray(T *_array, size_t _indices)
		{
			m_array = (T *)malloc(sizeof(T) * _indices);
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
			m_nextInsertPos = 0;
		}

		template <class T>
		DArray <T>::DArray(DArray<T> const &_array)
		{
			m_array = (T *)malloc(sizeof(T) * _array.m_arraySize);
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
			m_nextInsertPos = 0;
		}

		template <class T>
		DArray <T>::DArray(int _newStepSize)
		{
			if (_newStepSize < 1)
				m_stepSize = -1;
			else
				m_stepSize = _newStepSize;

			m_numUsed = m_arraySize = m_nextInsertPos = 0;
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
			if (m_arraySize < pageSizeInElements)
				return;
			if (m_numUsed > m_arraySize / 2)
				return;

			auto found = std::find(std::crbegin(m_shadow), std::crend(m_shadow), true);
			size_t idx = std::distance(found, std::crend(m_shadow));

			if (idx > m_arraySize)
				return;

			if ((m_arraySize - idx) < pageSizeInElements)
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
			m_numUsed = std::count(std::begin(m_shadow), std::end(m_shadow), true);
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
				T *newArray = (T *)malloc(sizeof(T) * newsize);

				// Zero-initialize all slots
				memset(newArray, 0, sizeof(T) * newsize);

				// Move any filled slots to new array.
				for (size_t idx = 0; idx < m_arraySize; idx++)
					newArray[idx] = std::move(m_array[idx]);

				m_arraySize = newsize;
				m_array = newArray;
			}
			else if (newsize < m_arraySize)
			{
				if (std::is_destructible<T>::value) {
					// Destroy any objects that are getting dropped off the end of the array.
					for (size_t idx = newsize; idx < m_arraySize; idx++)
						if (m_shadow[idx])
							std::destroy_at<T>(&m_array[idx]);
				}

				T *newArray = (T *)malloc(sizeof(T) * newsize);

				// Move any filled slots to new array.
				for (size_t idx = 0; idx < newsize; idx++)
					newArray[idx] = std::move(m_array[idx]);

				free(m_array);

				m_array = newArray;
				m_arraySize = newsize;

				/* We may have lost more than one node. It's worth rebuilding over. */
				recount();
				m_nextInsertPos = 0;
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
					setSize(32);
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
		size_t DArray <T>::allocate()
		{
			static_assert(std::is_constructible<T>::value);
			size_t freeslot = getNextFree();
			(void)(new (&m_array[freeslot]) T());
			m_shadow[freeslot] = true;
			m_numUsed++;
			return freeslot;
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
			if (std::is_destructible<T>::value) {
				for (size_t idx = 0; idx < m_arraySize; idx++) {
					if (m_shadow[idx])
						std::destroy_at<T>(&m_array[idx]);
				}
			}
			m_shadow.clear();

			m_numUsed = 0;
			m_nextInsertPos = 0;

			if (_freeMemory) {
				free(m_array);
				m_array = NULL;
				m_arraySize = 0;
			} else {
				m_shadow.resize( m_arraySize, false );
			}
		}

		template <class T>
		size_t DArray <T>::getNextFree()
		{
			if (!m_array || m_nextInsertPos >= m_arraySize)
				grow();

			size_t freeslot = (size_t)-1;

			/* Fast path: look at next linear insertion index */
			if (!m_shadow[m_nextInsertPos])
				freeslot = m_nextInsertPos;

			/* Slow path: find an empty spot somewhere in the array, and if we don't find one, grow. */
			while (freeslot == (size_t)-1) {
				/* We can start at m_nextInsertPos because it's always set to the most recent minimum free index */
				auto searchBegin = std::begin(m_shadow) + m_nextInsertPos;
				auto searchEnd = std::end(m_shadow);
				auto found = std::find(searchBegin, searchEnd, false);
				if (found != searchEnd)
					freeslot = std::distance(std::begin(m_shadow), found);
				else
					grow();
			}

			/* Remember the last insertion position, in case the next spot is open. */
			m_nextInsertPos = freeslot + 1;

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

			if ( std::is_destructible<T>::value ) {
				std::destroy_at<T>(&m_array[index]);
			}

			m_numUsed--;

			m_shadow[index] = false;
			m_nextInsertPos = std::min(index, m_nextInsertPos);
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

			T *temp_array = (T *)malloc(sizeof(T) * m_numUsed);

			memset(temp_array, 0, m_numUsed * sizeof(T));

			for (size_t i = 0; i < m_arraySize; i++) {
				if (valid(i)) {
					CoreAssert(idx < m_numUsed);
					temp_array[idx++] = m_array[i];
				}
			}

			ret = _sortMethod->Sort(temp_array, m_numUsed);

			m_shadow.clear();
			m_shadow.resize(m_numUsed, true);

			free(m_array);
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
