/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_hashtable_h
#error "This file shouldn't be compiled directly."
#endif

#include <crisscross/compare.h>
#include <crisscross/hash.h>
#include <crisscross/internal_mem.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>

#ifdef TARGET_OS_MACOSX
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace CrissCross
{
	namespace Data
	{
		template <class Data>
		HashTable<Data>::HashTable(uint64_t _initialSize)
		{
			/* This many elements should be enough for anybody. */
			m_size = ((uint32_t)-1) - 2; 
			m_slotsFree = m_size;
			m_data = (kvpair_t *)mmap(NULL, m_size * sizeof(kvpair_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);

			/* Assert that the memory was actually allocated. */
			CoreAssert(m_data != (kvpair_t *)-1);

			/* Assert that the default contents is zeroed. */
			CoreAssert(*((uint64_t *)m_data) == 0);
		}

		template <class Data>
		HashTable<Data>::~HashTable()
		{
			empty();
			munmap(m_data, m_size * sizeof(kvpair_t));
		}

		template <class Data>
		void HashTable<Data>::grow()
		{
			/* No. I refuse. */
			uint64_t YourHashTableIsTooDamnBig = 0;
			CoreAssert(YourHashTableIsTooDamnBig);
		}

		template <class Data>
		uint64_t HashTable<Data>::findInsertIndex(const char *_key) const
		{
			uint64_t index = Hash<const char *>(_key) % m_size;

			while (m_data[index].key != NULL)
			{
				CoreAssert(Compare<const char *>((const char *)m_data[index].key, _key) != 0);
				index++;
				if (index >= m_size)
					index = 0;
			}

			return index;
		}

		template <class Data>
		uint64_t HashTable<Data>::findIndex(const char *_key) const
		{
			uint64_t index = Hash<const char *>(_key) % m_size;
			kvpair_t *kvpair = &m_data[index];

			while (kvpair->key) {
				if (kvpair->key != (char *)-1 &&
				    Compare<const char *>(kvpair->key, _key) == 0)
					return index;

				index++;
				if (index >= m_size)
					index = 0;

				kvpair = &m_data[index];
			}

			return -1;
		}

		template <class Data>
		Data HashTable<Data>::find(const char * _key, Data const &_default) const
		{
			uint64_t index = findIndex(_key);
			if (index != (uint64_t)-1) {
				return m_data[index].data;
			}
			return _default;
		}

		template <class Data>
		bool HashTable<Data>::exists(const char * _key) const
		{
			return findIndex(_key) != -1;
		}

		template <class Data>
		bool HashTable<Data>::erase(const char *_key)
		{
			uint64_t index = findIndex(_key);
			if (index != (uint64_t)-1) {
				Dealloc(m_data[index].key);
				m_data[index].key = (char *)-1;
				m_usedIndices.erase(index);
				return true;
			}
			return false;
		}

		template <class Data>
		void HashTable<Data>::empty()
		{
			std::set<uint64_t>::iterator it;
			for (it = m_usedIndices.begin();
			     it != m_usedIndices.end();
				 it++)
			{
				kvpair_t *kvpair = &m_data[*it];
				if (kvpair->key != NULL &&
				    kvpair->key != (char *)-1)
					Dealloc(kvpair->key);
				kvpair->key = NULL;
			}
		}

		template <class Data>
		uint64_t HashTable<Data>::insert(const char *_key, Data const &_data)
		{
			if (m_slotsFree < m_size / 4) {
				grow();
			}

			uint64_t index = findInsertIndex(_key);
			CoreAssert(m_data[index].key == NULL || m_data[index].key == (char*)-1);
			m_data[index].key = cc_strdup(_key);
			m_data[index].data = _data;
			m_slotsFree--;

			m_usedIndices.insert(index);

			return index;
		}
	}
}
