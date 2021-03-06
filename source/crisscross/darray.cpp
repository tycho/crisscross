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
			m_stepSize = -1;
			m_numUsed = m_arraySize = m_insertPos = 0;
			m_array = NULL;
			m_emptyNodes = new DStack<size_t>;
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
					m_insertPos = i + 1;
				}
			}

			m_arraySize = _indices;
			m_stepSize = -1;
			m_emptyNodes = new DStack<size_t>;

			rebuildStack();
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
			m_insertPos = _array.m_insertPos;
			m_emptyNodes = new DStack<size_t>;

			rebuildStack();
		}

		template <class T>
		DArray <T>::DArray(int _newStepSize)
		{
			if (_newStepSize < 1)
				m_stepSize = -1;
			else
				m_stepSize = _newStepSize;

			m_numUsed = m_arraySize = m_insertPos = 0;
			m_array = NULL;
			m_emptyNodes = new DStack<size_t> (_newStepSize + 1);
		}

		template <class T>
		DArray <T>::~DArray()
		{
			empty();
			delete m_emptyNodes;
			m_emptyNodes = NULL;
		}

		template <class T>
		void DArray <T>::disableFreeList()
		{
			delete m_emptyNodes;
			m_emptyNodes = NULL;
		}

		template <class T>
		void DArray <T>::rebuildStack()
		{
			if (!m_emptyNodes)
				return;

			/*  Reset free list */
			m_emptyNodes->empty(false);

			/* Step through, rebuilding */
			for (size_t i = m_insertPos - 1; (int)i >= 0; i--)
				if (!m_shadow[i])
					m_emptyNodes->push(i);
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
			setSize(newsize);
			m_shadow.clear();
			m_shadow.resize(newsize, true);
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
				rebuildStack();
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
			const size_t growthHeuristic = 4096 / sizeof(void *);

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
			return freeslot;
		}

		template <class T>
		void DArray <T>::insert(T const & newdata, size_t index)
		{
			while (index >= m_arraySize)
				grow();
			if (index + 1 > m_insertPos)
				m_insertPos = index + 1;
			m_array[index] = newdata;
			if (!m_shadow[index]) {
				/* Nasty because we took an element that's on the empty
				 * node stack somewhere. We have to rebuild the stack. :(
				 */
				m_shadow[index] = true;
				m_numUsed++;
				rebuildStack();
			}
		}

		template <class T>
		void DArray <T>::empty(bool _freeMemory)
		{
			m_shadow.clear();

			m_numUsed = 0;
			m_insertPos = 0;

			if (_freeMemory) {
				if (m_emptyNodes)
					m_emptyNodes->empty();
				m_shadow.resize(0);
				delete [] m_array;
				m_array = NULL;
				m_arraySize = 0;
			} else {
				rebuildStack();
			}
		}

		template <class T>
		size_t DArray <T>::getNextFree()
		{
			/* WARNING: This function assumes the node returned */
			/*          will be used by the calling function. */

			if (!m_array)
				grow();

			size_t freeslot = (size_t)-1;

			while (m_emptyNodes && m_emptyNodes->count()) {
				freeslot = m_emptyNodes->pop();
				if (!m_shadow[freeslot])
					break;
				freeslot = -1;
			}

			if (freeslot == (size_t)-1)
				freeslot = m_insertPos++;

			while (freeslot >= m_arraySize)
				grow();

			if (!m_shadow[freeslot])
				m_numUsed++;

			m_shadow[freeslot] = true;

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
			m_shadow[index] = 1;
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

			if (m_emptyNodes)
				m_emptyNodes->push(index);

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

			m_insertPos = m_arraySize = m_numUsed;

			rebuildStack();
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
