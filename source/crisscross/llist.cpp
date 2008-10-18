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
			m_next = NULL;
			m_previous = NULL;
		}

		template <class T> LList <T>::LList()
		{
			m_first = NULL;
			m_last = NULL;
			m_numItems = 0;
			m_previous = NULL;
			m_previousIndex = (size_t)-1;
		}

		template <class T> LList <T>::~LList()
		{
			empty();
		}

		template <class T> LList <T>::LList(const LList <T> &source) : m_first(NULL),
			m_last(NULL),
			m_previous(NULL),
			m_previousIndex(-1),
			m_numItems(0)
		{
			for (size_t i = 0; i < source.size(); i++) {
				insert_back(source.GetData(i));
			}
		}

		template <class T>
		LList <T> &LList <T>::operator =(const LList <T> &source)
		{
			empty();
			for (size_t i = 0; i < source.size(); i++) {
				insert_back(source.getData(i));
			}

			return *this;
		}

		template <class T> void LList <T>::change(const T &_rec, size_t _index)
		{
			LListNode <T> *li = getItem(_index);
			li->m_data = _rec;
		}

		template <class T> void LList <T>::insert(const T & newdata)
		{
			insert_back(newdata);
		}


		template <class T> void LList <T>::insert_back(const T & newdata)
		{
			LListNode <T> *li = new LListNode <T> ();
			li->m_data = newdata;
			li->m_next = NULL;
			li->m_previous = m_last;
			++m_numItems;

			if (m_last == NULL) {
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
			li->m_previous = NULL;
			++m_numItems;

			if (m_last == NULL) {
				li->m_next = NULL;
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
		void LList <T>::insert_at(const T & newdata, size_t index)
		{
			if (index == 0) {
				insert_front(newdata);
			} else if (index == m_numItems) {
				insert_back(newdata);
			} else {
				LListNode <T> *current = m_first;

				for (size_t i = 0; i < index - 1; ++i) {
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

		template <class T> size_t LList <T>::size() const
		{
			return m_numItems;
		}

		template <class T> T const & LList <T>::get(size_t index) const
		{
			LListNode <T> const *item = getItem(index);

			if (item) {
				T const &ret = item->m_data;
				return ret;
			}

			/* Below wastes memory -- need a cleaner way to error out. */
			static T             nullVar(0);
			return nullVar;
		}

		template <class T> T * LList <T>::getPointer(size_t index) const
		{
			LListNode <T> *item = getItem(index);
			if (item) {
				T *ret = &item->m_data;
				return ret;
			}

			return NULL;
		}

		template <class T> LListNode <T> *LList <T>::getItem(size_t index) const
		{
			if (!valid(index)) {
				return NULL;
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
			} else if ((size_t)abs((long)index - (long)m_previousIndex) > (m_numItems - index)) {
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

		template <class T> bool LList <T>::valid(size_t index) const
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

			m_first = NULL;
			m_last = NULL;
			m_numItems = 0;
			m_previous = NULL;
			m_previousIndex = (size_t)-1;
		}

		template <class T> void LList <T>::remove(size_t index)
		{
			LListNode <T> *current = getItem(index);

			if (current == NULL) {
				return;
			}

			if (current->m_previous == NULL)
				m_first = current->m_next;
			else
				current->m_previous->m_next = current->m_next;

			if (current->m_next == NULL)
				m_last = current->m_previous;
			else
				current->m_next->m_previous = current->m_previous;

			if (index == m_previousIndex) {
				if (m_numItems == 1) {
					m_previousIndex = (size_t)-1;
					m_previous = NULL;
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
		size_t LList<T>::mem_usage() const
		{
			LListNode<T> *node = m_first;
			size_t        ret = sizeof(*this);
			while (node) {
				ret += sizeof(*node);
				node = node->m_next;
			}
			return ret;
		}

#ifdef ENABLE_SORTS
		template <class T> void LList <T>::sort(CrissCross::Data::Sorter<T> &_sortMethod)
		{
			sort(&_sortMethod);
		}

		template <class T> void LList <T>::sort(CrissCross::Data::Sorter<T> *_sortMethod)
		{
			/*
			 * The theory here is that doing LList node relinking is both
			 * painful and costly, as the relinking may be done more than
			 * one relink on any one node because of the way certain sorts
			 * work. So it simplifies things by serializing the LList as a
			 * DArray temporarily and then rebuilding the LList.
			 *
			 * Unfortunately this method is still very costly, so if someone
			 * is doing a LList::sort, they might as well be using a DArray.
			 *
			 */
			size_t     llistSize = size();
			DArray <T> sortArray;
			sortArray.setSize(llistSize);
			for (size_t i = 0; i < llistSize; i++) {
				sortArray.insert(get(i));
			}

			empty();
			sortArray.sort(_sortMethod);
			for (size_t i = 0; i < llistSize; i++) {
				insert(sortArray.get(i));
			}
		}
#endif

		template <class T> T const & LList <T>::operator [](size_t index) const
		{
			LListNode<T> *item = getItem(index);
			return item->m_data;
		}

		template <class T> T & LList <T>::operator [](size_t index)
		{
			LListNode<T> *item = getItem(index);
			return item->m_data;
		}

		template <class T> size_t LList <T>::find(const T & data)
		{
			size_t const size = this->size();

			for (size_t i = 0; i < size; ++i) {
				if (Compare(get(i), data) == 0)	{
					return i;
				}
			}

			return -1;
		}
	}
}
