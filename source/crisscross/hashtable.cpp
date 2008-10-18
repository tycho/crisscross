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

#ifndef __included_cc_hashtable_h
#error "This file shouldn't be compiled directly."
#endif

#include <crisscross/hash.h>

namespace CrissCross
{
	namespace Data
	{
		template <class Key, class Data>
		HashTable<Key, Data>::HashTable(size_t _initialSize)
		{
			if (_initialSize < 25) _initialSize = 25;

			m_size = _initialSize;
			m_used = 0;
			m_array = new tree_t *[_initialSize];
			memset(m_array, 0, sizeof(tree_t *) * _initialSize);
		}

		template <class Key, class Data>
		HashTable<Key, Data>::~HashTable()
		{
			for (size_t i = 0; i < m_size; i++) {
				delete m_array[i];
				m_array[i] = NULL;
			}

			delete [] m_array;
			m_array = NULL;
			m_size = 0;
		}

		template <class Key, class Data>
		size_t HashTable<Key, Data>::findIndex(Key const &_key) const
		{
			size_t khash = Hash(_key);
			size_t pos = khash % m_size;
			return pos;
		}

		template <class Key, class Data>
		Data HashTable<Key, Data>::find(Key const & _key) const
		{
			static Data null(0);
			size_t      index = findIndex(_key);
			if (!m_array[index]) return null;

			Data        ret;
			if (m_array[index]->find(_key, ret)) {
				return ret;
			} else {
				return null;
			}
		}

		template <class Key, class Data>
		bool HashTable<Key, Data>::exists(Key const & _key) const
		{
			size_t index = findIndex(_key);
			if (!m_array[index]) return false;

			return m_array[index]->exists(_key);
		}

		template <class Key, class Data>
		bool HashTable<Key, Data>::find(Key const &_key, Data &_data) const
		{
			size_t index = findIndex(_key);
			if (!m_array[index]) return false;

			return m_array[index]->find(_key, _data);
		}

		template <class Key, class Data>
		bool HashTable<Key, Data>::erase(Key const &_key)
		{
			size_t index = findIndex(_key);
			if (!m_array[index]) return false;

			return m_array[index]->erase(_key);
		}

		template <class Key, class Data>
		size_t HashTable<Key, Data>::mem_usage() const
		{
			size_t ret = sizeof(*this);
			for (unsigned int i = 0; i < m_size; i++)
				if (m_array[i])
					ret += m_array[i]->mem_usage();

			return ret;
		}

		template <class Key, class Data>
		bool HashTable<Key, Data>::insert(Key const &_key, Data const &_data)
		{
			size_t khash = Hash(_key);
			size_t pos = khash % m_size;

			if (!m_array[pos])
				m_array[pos] = new tree_t();

			CoreAssert(m_array[pos]);

			return m_array[pos]->insert(_key, _data);
		}
	}
}
