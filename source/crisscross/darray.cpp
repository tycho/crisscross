/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
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
			m_array = nullptr;
		}

		template <class T>
		DArray <T>::~DArray()
		{
			empty();
		}

		template <class T>
		void DArray<T>::defragment()
		{
			auto itFirstInvalid = std::find(std::begin(m_shadow), std::end(m_shadow), false);
			uint32_t idxFirstInvalid = std::distance(std::begin(m_shadow), itFirstInvalid);
			if (idxFirstInvalid >= m_numUsed)
				return;

			auto itLastValid = std::find(std::crbegin(m_shadow), std::crend(m_shadow), true);
			uint32_t idxLastValid = std::distance(itLastValid, std::crend(m_shadow));
			if (idxLastValid == 0)
				return;

			for (uint32_t i = idxFirstInvalid; i < idxLastValid; i++) {
				if (!valid(i)) {
					for (; idxLastValid > i;) {
						if (valid(idxLastValid)) {
							m_array[i] = std::move(m_array[idxLastValid]);
							m_shadow[i] = true;
							m_shadow[idxLastValid] = false;
							idxLastValid--;
							break;
						}
						idxLastValid--;
					}
				}
			}
		}

		template <class T>
		void DArray <T>::compact()
		{
			constexpr uint32_t pageSizeInElements = std::max((uint32_t)(4096u / sizeof(T)), (uint32_t)32u);

			/* Only resize if it would possibly be beneficial. */
			if (m_arraySize < pageSizeInElements)
				return;
			if (m_numUsed > m_arraySize / 2)
				return;

			auto found = std::find(std::crbegin(m_shadow), std::crend(m_shadow), true);
			uint32_t idx = std::distance(found, std::crend(m_shadow));

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
		void DArray <T>::minimize()
		{
			/* Early out if we're already at the smallest size we can be. */
			if (m_numUsed == m_arraySize)
				return;

			auto found = std::find(std::crbegin(m_shadow), std::crend(m_shadow), true);
			uint32_t idx = std::distance(found, std::crend(m_shadow));

			if (idx >= m_arraySize)
				return;

			setSize(idx);
		}

		template <class T>
		void DArray <T>::recount()
		{
			m_numUsed = std::count(std::begin(m_shadow), std::end(m_shadow), true);
		}

		template <class T>
		void DArray <T>::reserve(uint32_t newsize)
		{
			if (newsize > m_arraySize) {
				setSize(newsize);
				m_shadow.clear();
				m_shadow.resize(newsize, true);
			}
		}

		template <class T>
		void DArray <T>::setMinimumSize(uint32_t newsize)
		{
			if (m_arraySize < newsize)
				setSize(newsize);
		}

		template <class T>
		void DArray <T>::setSize(uint32_t newsize)
		{
			if (newsize > m_arraySize) {
				T *newArray = (T *)malloc(sizeof(T) * newsize);

				/* Move any filled slots to new array. */
				for (uint32_t idx = 0; idx < m_arraySize; idx++)
					if (m_shadow[idx])
						newArray[idx] = std::move(m_array[idx]);

				free(m_array);

				m_shadow.resize(newsize, false);
				m_arraySize = newsize;
				m_array = newArray;
			}else if (newsize < m_arraySize) {
				if (std::is_destructible<T>::value && !std::is_trivially_destructible<T>::value) {
					/* Destroy any objects that are getting dropped off the end of the array. */
					for (uint32_t idx = newsize; idx < m_arraySize; idx++)
						if (m_shadow[idx])
#if __cplusplus >= 201703L
							std::destroy_at<T>(&m_array[idx]);
#else
							((T *)(&m_array[idx]))->~T();
#endif
				}

				T *newArray = newsize > 0 ? (T *)malloc(sizeof(T) * newsize) : nullptr;

				/* Move any filled slots to new array. */
				for (uint32_t idx = 0; idx < newsize; idx++)
					if (m_shadow[idx])
						newArray[idx] = std::move(m_array[idx]);

				free(m_array);

				m_shadow.resize(newsize, false);
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
			constexpr uint32_t growthHeuristic = (16 * 1024 * 1024) / sizeof(T);

			if (m_stepSize == -1) {
				if (m_arraySize == 0) {
					setSize(32);
				} else if (m_arraySize <= growthHeuristic) {
					setSize(m_arraySize * 2);
				} else {
					setSize((uint32_t)(m_arraySize * 1.5));
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
		uint32_t DArray <T>::allocate()
		{
			static_assert(std::is_constructible<T>::value);
			uint32_t freeslot = getNextFree();
			(void)(new (&m_array[freeslot]) T());
			m_shadow[freeslot] = true;
			m_numUsed++;
			return freeslot;
		}

		template <class T>
		uint32_t DArray <T>::insert(T const & newdata)
		{
			uint32_t freeslot = getNextFree();
			m_array[freeslot] = newdata;
			m_shadow[freeslot] = true;
			m_numUsed++;
			return freeslot;
		}

		template <class T>
		void DArray <T>::insert(T const & newdata, uint32_t index)
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
			if (std::is_destructible<T>::value && !std::is_trivially_destructible<T>::value) {
				for (uint32_t idx = 0; idx < m_arraySize; idx++) {
					if (m_shadow[idx])
#if __cplusplus >= 201703L
						std::destroy_at<T>(&m_array[idx]);
#else
						((T *)(&m_array[idx]))->~T();
#endif
				}
			}
			m_shadow.clear();

			m_numUsed = 0;
			m_nextInsertPos = 0;

			if (_freeMemory) {
				free(m_array);
				m_array = nullptr;
				m_arraySize = 0;
			} else {
				m_shadow.resize(m_arraySize, false);
			}
		}

		template <class T>
		uint32_t DArray <T>::getNextFree()
		{
			if (!m_array || m_nextInsertPos >= m_arraySize)
				grow();

			uint32_t freeslot = (uint32_t)-1;

			/* Fast path: look at next linear insertion index */
			if (!m_shadow[m_nextInsertPos])
				freeslot = m_nextInsertPos;

			/* Slow path: find an empty spot somewhere in the array, and if we don't find one, grow. */
			while (freeslot == (uint32_t)-1) {
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
		T DArray <T>::get(uint32_t index) const
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);

			return m_array[index];
		}

		template <class T>
		T & DArray <T>::operator [](uint32_t index)
		{
			CoreAssert(index < m_arraySize);
			if (!m_shadow[index]) {
				m_shadow[index] = true;
				m_numUsed++;
			}
			return m_array[index];
		}

		template <class T>
		const T &DArray <T>::operator [](uint32_t index) const
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);

			return m_array[index];
		}

		template <class T>
		uint32_t DArray<T>::mem_usage() const
		{
			uint32_t ret = sizeof(*this);
			ret += m_arraySize * sizeof(T);
			return ret;
		}

		template <class T>
		void DArray <T>::remove(uint32_t index)
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);

			if (std::is_destructible<T>::value && !std::is_trivially_destructible<T>::value) {
#if __cplusplus >= 201703L
				std::destroy_at<T>(&m_array[index]);
#else
				((T *)(&m_array[index]))->~T();
#endif
			}

			m_numUsed--;

			m_shadow[index] = false;
			m_nextInsertPos = std::min(index, m_nextInsertPos);
		}

		template <class T>
		uint32_t DArray <T>::find(T const &_query) const
		{
			for (uint32_t a = 0; a < m_arraySize; ++a)
				if (m_shadow[a])
					if (m_array[a] == _query)
						return a;
			return -1;
		}

		template <class T>
		int DArray <T>::sort(Sorter<T> *_sortMethod)
		{
			uint32_t idx = 0;
			int ret;

			if (m_numUsed < 2)
				return 0;

			T *temp_array = (T *)malloc(sizeof(T) * m_numUsed);

			memset(temp_array, 0, m_numUsed * sizeof(T));

			for (uint32_t i = 0; i < m_arraySize; i++) {
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
			for (uint32_t i = 0; i < m_arraySize; ++i) {
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
			for (uint32_t i = 0; i < m_arraySize; ++i) {
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
			for (uint32_t i = 0; i < m_arraySize; ++i) {
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
			for (uint32_t i = 0; i < m_arraySize; ++i) {
				if (valid(i)) {
					delete [] m_array[i];
				}
			}
			empty();
		}

		template <class T>
		void DArray<T>::ChangeData(T const & _rec, uint32_t index)
		{
			CoreAssert(index < m_arraySize);
			CoreAssert(m_shadow[index]);
			m_array[index] = _rec;
		}
#endif
	}
}
