/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_hashtable_h
#error "This file shouldn't be compiled directly."
#endif

#include <crisscross/compare.h>
#include <crisscross/hash.h>
#include <crisscross/internal_mem.h>

namespace CrissCross
{
	namespace Data
	{
		template <class Data>
		HashTable<Data>::HashTable(size_t _initialSize)
		: m_keys(NULL), m_size(32)
		{
			m_mask = m_size - 1;
			m_slotsFree = m_size;
			m_keys = new char *[m_size];
			m_data = new Data[m_size];

			memset(m_keys, 0, sizeof(char *) * m_size);
			memset(m_data, 0, sizeof(Data) * m_size);
		}

		template <class Data>
		HashTable<Data>::~HashTable()
		{
			empty();

			delete [] m_keys;
			delete [] m_data;
		}

		template <class Data>
		void HashTable<Data>::grow()
		{
			unsigned int oldSize = m_size;
			m_size *= 2;
			m_mask = m_size - 1;
			char **oldKeys = m_keys;
			m_keys = new char *[m_size];
			Data *oldData = m_data;
			m_data = new Data[m_size];

			memset(m_keys, 0, sizeof(char *) * m_size);
			memset(m_data, 0, sizeof(Data) * m_size);

			for (unsigned int i = 0; i < oldSize; ++i) {
				if (oldKeys[i] != NULL) {
					unsigned int newIndex = findInsertIndex(oldKeys[i]);
					m_keys[newIndex] = oldKeys[i];
					m_data[newIndex] = oldData[i];
				}
			}

			m_slotsFree += m_size - oldSize;

			delete [] oldKeys;
			delete [] oldData;
		}

		template <class Data>
		size_t HashTable<Data>::findInsertIndex(const char *_key) const
		{
			unsigned int index = Hash<const char *>(_key) & m_mask;

			while (m_keys[index] != NULL &&
			       m_keys[index] != (char *)-1)
			{
				CoreAssert(Compare<const char *>((const char *)m_keys[index], _key) != 0);
				index++;
				index &= m_mask;
			}

			return index;
		}

		template <class Data>
		size_t HashTable<Data>::findIndex(const char *_key) const
		{
			size_t index = Hash<const char *>(_key) & m_mask;

			if (m_keys[index] == NULL &&
			    m_keys[index] != (char*)-1) {
				return -1;
			}

			while (true)
			{
				if (m_keys[index] != NULL && m_keys[index] != (char*)-1) {
					if (Compare<const char *>((const char *)m_keys[index], _key) == 0)
						break;
				}

				index++;
				index &= m_mask;

				if (m_keys[index] == NULL &&
				    m_keys[index] != (char*)-1 ) {
					return -1;
				}
			}

			return index;
		}

		template <class Data>
		Data HashTable<Data>::find(const char * _key, Data const &_default) const
		{
			size_t index = findIndex(_key);
			if (index != (size_t)-1) {
				return m_data[index];
			}
			return _default;
		}

		template <class Data>
		bool HashTable<Data>::exists(const char * _key) const
		{
			return findIndex(_key) != (size_t)-1;
		}

		template <class Data>
		bool HashTable<Data>::erase(const char *_key)
		{
			size_t index = findIndex(_key);
			if (index != (size_t)-1) {
				Dealloc(m_keys[index]);
				m_keys[index] = (char*)-1;
				m_slotsFree++;
				return true;
			}
			return false;
		}

		template <class Data>
		void HashTable<Data>::empty()
		{
			for (size_t i = 0; i < m_size; ++i) {
				if (m_keys[i] == (char*)-1) m_keys[i] = NULL;
				Dealloc(m_keys[i]);
			}

			memset(m_keys, 0, sizeof(char *) * m_size);
			memset(m_data, 0, sizeof(Data) * m_size);
		}

		template <class Data>
		bool HashTable<Data>::insert(const char *_key, Data const &_data)
		{
			if (m_slotsFree * 2 <= m_size) {
				grow();
			}

			unsigned int index = findInsertIndex(_key);
			CoreAssert(m_keys[index] == NULL || m_keys[index] == (char*)-1);
			m_keys[index] = cc_strdup(_key);
			m_data[index] = _data;
			m_slotsFree--;

			return index;
		}
	}
}
