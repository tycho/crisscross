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

#ifndef __included_cc_dstack_h
#error "This file shouldn't be compiled directly."
#endif

#include <crisscross/internal_mem.h>
#include <crisscross/dstack.h>

namespace CrissCross
{
	namespace Data
	{
		template <class T>
		DStack<T>::DStack(size_t N)
		{
			if (N < 1)
				m_stepSize = (size_t)-1;
			else
				m_stepSize = N;

			m_top = m_bottom = nullptr;
			m_size = 0;
		}

		template <class T>
		DStack<T>::~DStack()
		{
			delete [] m_bottom;
			m_top = m_bottom = nullptr;
		}

		template <class T>
		void DStack<T>::setSize(size_t _size)
		{
			/* This function is ONLY stable for increases in size, not decreases. */
			T *newstack_ = nullptr;
			newstack_ = new T[_size];
			if (m_bottom) {
				memcpy(newstack_, m_bottom, sizeof(T) * m_size);
				delete [] m_bottom;
			}

			m_bottom = newstack_;
			m_top = m_bottom + m_size;
			m_size = _size;
		}

		template <class T>
		void DStack<T>::grow()
		{
			if (m_stepSize == (size_t)-1) {
				if (m_size == 0) {
					setSize(64);
				} else {
					setSize(m_size * 2);
				}
			} else {
				setSize(m_size + m_stepSize);
			}
		}

		template <class T>
		void DStack<T>::push(T const &val)
		{
			if (count() == m_size) {                /* the stack is full. need more space! */
				grow();
			}

			*m_top = val;
			m_top++;
		}

		template <class T>
		size_t DStack<T>::count() const
		{
			return (m_top - m_bottom);
		}

		template <class T>
		T DStack<T>::pop()
		{
			CoreAssert(m_top);

			m_top--;
			T ret = *m_top;
			return ret;
		}

		template <class T>
		T DStack<T>::peek() const
		{
			CoreAssert(m_top);

			const T &ret = *(m_top - 1);
			return ret;
		}

		template <class T>
		void DStack<T>::empty(bool _freeMemory)
		{
			if (_freeMemory) {
				delete [] m_bottom;
				m_top = m_bottom = nullptr;
				m_size = 0;
			} else {
				m_top = m_bottom;
			}
		}
	}
}
