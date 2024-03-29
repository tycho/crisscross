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

#ifndef __included_cc_llist_h
#error "This file shouldn't be compiled directly."
#endif

#include <crisscross/llist.h>

namespace CrissCross
{
	namespace Data
	{
		template <class T> LListNode <T>::LListNode()
		{
			m_next = nullptr;
			m_previous = nullptr;
		}

		template <class T> LList <T>::LList()
		{
			m_first = nullptr;
			m_last = nullptr;
			m_numItems = 0;
			m_previous = nullptr;
			m_previousIndex = (uint32_t)-1;
		}

		template <class T> LList <T>::~LList()
		{
			empty();
		}

		template <class T> LList <T>::LList(const LList <T> &source) : m_first(nullptr),
			m_last(nullptr),
			m_previous(nullptr),
			m_previousIndex(-1),
			m_numItems(0)
		{
			LListNode <T> *li;
			for (uint32_t i = 0; i < source.size(); i++) {
				li = source.getItem(i);
				insert_back(li->m_data);
			}
		}

		template <class T>
		LList <T> &LList <T>::operator =(const LList <T> &source)
		{
			LListNode <T> *li;
			empty();
			for (uint32_t i = 0; i < source.size(); i++) {
				li = source.getItem(i);
				insert_back(li->m_data);
			}

			return *this;
		}

		template <class T> void LList <T>::change(const T &_rec, uint32_t _index)
		{
			LListNode <T> *li = getItem(_index);
			li->m_data = _rec;
		}

		template <class T> void LList <T>::insert(const T & newdata)
		{
			insert_back(newdata);
		}

		template <class T> T &LList <T>::insert_new()
		{
			LListNode <T> *li = new LListNode <T> ();
			li->m_next = nullptr;
			li->m_previous = m_last;
			++m_numItems;

			if (m_last == nullptr) {
				m_first = li;
				m_last = li;

				m_previous = li;
				m_previousIndex = 0;
			} else {
				m_last->m_next = li;
				m_last = li;
			}

			return li->m_data;
		}

		template <class T> void LList <T>::insert_back(const T & newdata)
		{
			LListNode <T> *li = new LListNode <T> ();
			li->m_data = newdata;
			li->m_next = nullptr;
			li->m_previous = m_last;
			++m_numItems;

			if (m_last == nullptr) {
				m_first = li;
				m_last = li;

				m_previous = li;
				m_previousIndex = 0;
			} else {
				m_last->m_next = li;
				m_last = li;
			}
		}

		template <class T> void LList <T>::insert_front(const T & newdata)
		{
			LListNode <T> *li = new LListNode <T> ();
			li->m_data = newdata;
			li->m_previous = nullptr;
			++m_numItems;

			if (m_last == nullptr) {
				li->m_next = nullptr;
				m_first = li;
				m_last = li;

				m_previous = li;
				m_previousIndex = 0;
			} else {
				m_first->m_previous = li;
				li->m_next = m_first;
				m_first = li;

				m_previousIndex++;
			}
		}

		template <class T>
		void LList <T>::insert_at(const T & newdata, uint32_t index)
		{
			if (index == 0) {
				insert_front(newdata);
			} else if (index == m_numItems) {
				insert_back(newdata);
			} else {
				LListNode <T> *current = m_first;

				for (uint32_t i = 0; i < index - 1; ++i) {
					if (!current) {
						return;
					}

					current = current->m_next;
				}

				if (!current) {
					return;
				}

				LListNode <T> *li = new LListNode <T> ();
				li->m_data = newdata;
				li->m_previous = current;
				li->m_next = current->m_next;
				if (current->m_next)
					current->m_next->m_previous = li;

				current->m_next = li;
				++m_numItems;

				m_previousIndex = 0;
				m_previous = m_first;
			}
		}

		template <class T> uint32_t LList <T>::size() const
		{
			return m_numItems;
		}

		template <class T> T const & LList <T>::get(uint32_t index, T const &_default) const
		{
			LListNode <T> const *item = getItem(index);

			if (item) {
				T const &ret = item->m_data;
				return ret;
			}

			return _default;
		}

		template <class T> T * LList <T>::getPointer(uint32_t index) const
		{
			LListNode <T> *item = getItem(index);
			if (item) {
				T *ret = &item->m_data;
				return ret;
			}

			return nullptr;
		}

		template <class T> LListNode <T> *LList <T>::getItem(uint32_t index) const
		{
			if (!valid(index)) {
				return nullptr;
			}

			/* */
			/* Choose a place for which to start walking the list */

			/* Best place to start is either; m_first, m_previous or m_last */
			/* */
			/* m_first                m_previous                                   m_last */
			/*     |----------:-----------|---------------------:--------------------| */
			/*            mid-point 1                      mid-point 2 */
			/* */
			/* If index is less than mid-point 1, then m_first is nearest. */
			/* If index is greater than mid-point 2, then m_last is nearest. */
			/* Otherwise m_previous is nearest. */
			/* The two if statements below test for these conditions. */


			if (index <= (m_previousIndex >> 1)) {
				m_previous = m_first;
				m_previousIndex = 0;
			} else if ((uint32_t)labs((long)index - (long)m_previousIndex) > (m_numItems - index)) {
				m_previous = m_last;
				m_previousIndex = m_numItems - 1;
			}

			if (!m_previous) {
				m_previous = m_first;
				m_previousIndex = 0;
			}

			while (index > m_previousIndex)	{
				m_previous = m_previous->m_next;
				m_previousIndex++;
			}

			while (index < m_previousIndex)	{
				m_previous = m_previous->m_previous;
				m_previousIndex--;
			}

			LListNode<T> *temp = m_previous;

			return temp;
		}

		template <class T> bool LList <T>::valid(uint32_t index) const
		{
			return (index < m_numItems);
		}

		template <class T> void LList <T>::empty()
		{
			LListNode <T> *current = m_first;
			while (current)	{
				LListNode <T> *m_next = current->m_next;
				delete current;

				current = m_next;
			}

			m_first = nullptr;
			m_last = nullptr;
			m_numItems = 0;
			m_previous = nullptr;
			m_previousIndex = (uint32_t)-1;
		}

		template <class T> void LList <T>::remove(uint32_t index)
		{
			LListNode <T> *current = getItem(index);

			if (current == nullptr) {
				return;
			}

			if (current->m_previous == nullptr)
				m_first = current->m_next;
			else
				current->m_previous->m_next = current->m_next;

			if (current->m_next == nullptr)
				m_last = current->m_previous;
			else
				current->m_next->m_previous = current->m_previous;

			if (index == m_previousIndex) {
				if (m_numItems == 1) {
					m_previousIndex = (uint32_t)-1;
					m_previous = nullptr;
				} else if (index > 0) {
					m_previousIndex--;
					m_previous = current->m_previous;
				} else {
					m_previous = current->m_next;
				}
			} else if (index < m_previousIndex) {
				m_previousIndex--;
			}

			delete current;

			--m_numItems;
		}

		template <class T>
		uint32_t LList<T>::mem_usage() const
		{
			LListNode<T> *node = m_first;
			uint32_t ret = sizeof(*this);
			while (node) {
				ret += sizeof(*node);
				node = node->m_next;
			}
			return ret;
		}

		template <class T> T const & LList <T>::operator [](uint32_t index) const
		{
			LListNode<T> *item = getItem(index);
			return item->m_data;
		}

		template <class T> T & LList <T>::operator [](uint32_t index)
		{
			LListNode<T> *item = getItem(index);
			return item->m_data;
		}

		template <class T> uint32_t LList <T>::find(const T & data)
		{
			uint32_t const lsize = this->size();

			if (Compare(get(m_previousIndex), data) == 0)
				return m_previousIndex;

			for (uint32_t i = 0; i < lsize; ++i) {
				if (Compare(get(i), data) == 0)	{
					return i;
				}
			}

			return -1;
		}
	}
}
