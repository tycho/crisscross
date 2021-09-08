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
		template <class Data, bool OwnsKeys>
		HashTable<Data, OwnsKeys>::HashTable(size_t _initialSize)
		: m_keys(nullptr), m_size(32)
		{
			static_assert(std::is_trivially_copyable<Data>::value);
			m_mask = m_size - 1;
			m_slotsFree = m_size;
			m_keys = new const char *[m_size];
			m_data = new Data[m_size];

			memset(m_keys, 0, sizeof(const char *) * m_size);
			memset(m_data, 0, sizeof(Data) * m_size);
		}

		template <class Data, bool OwnsKeys>
		HashTable<Data, OwnsKeys>::~HashTable()
		{
			empty();

			delete [] m_keys;
			delete [] m_data;
		}

		template <class Data, bool OwnsKeys>
		void HashTable<Data, OwnsKeys>::grow()
		{
			unsigned int oldSize = m_size;
			m_size *= 2;
			m_mask = m_size - 1;
			const char **oldKeys = m_keys;
			m_keys = new const char *[m_size];
			Data *oldData = m_data;
			m_data = new Data[m_size];

			memset(m_keys, 0, sizeof(char *) * m_size);
			memset(m_data, 0, sizeof(Data) * m_size);

			for (size_t i = 0; i < oldSize; ++i) {
				if (oldKeys[i] != nullptr) {
					size_t newIndex = findInsertIndex(oldKeys[i]);
					m_keys[newIndex] = oldKeys[i];
					m_data[newIndex] = oldData[i];
				}
			}

			m_slotsFree += m_size - oldSize;

			delete [] oldKeys;
			delete [] oldData;
		}

		template <class Data, bool OwnsKeys>
		size_t HashTable<Data, OwnsKeys>::findInsertIndex(const char *_key) const
		{
			size_t index = Hash<const char *>(_key) & m_mask;

			while (m_keys[index] != nullptr &&
			       m_keys[index] != (char *)-1)
			{
				CoreAssert(Compare<const char *>((const char *)m_keys[index], _key) != 0);
				index++;
				index &= m_mask;
			}

			return index;
		}

		template <class Data, bool OwnsKeys>
		size_t HashTable<Data, OwnsKeys>::findIndex(const char *_key) const
		{
			size_t index = Hash<const char *>(_key) & m_mask;

			if (m_keys[index] == nullptr) {
				return -1;
			}

			while (true)
			{
				if (m_keys[index] != nullptr && m_keys[index] != (char*)-1) {
					if (Compare<const char *>((const char *)m_keys[index], _key) == 0)
						break;
				}

				index++;
				index &= m_mask;

				if (m_keys[index] == nullptr) {
					return -1;
				}
			}

			return index;
		}

		template <class Data, bool OwnsKeys>
		Data HashTable<Data, OwnsKeys>::find(const char * _key, Data const &_default) const
		{
			size_t index = findIndex(_key);
			if (index != (size_t)-1) {
				return m_data[index];
			}
			return _default;
		}

		template <class Data, bool OwnsKeys>
		bool HashTable<Data, OwnsKeys>::exists(const char * _key) const
		{
			return findIndex(_key) != -1;
		}

		template <class Data, bool OwnsKeys>
		bool HashTable<Data, OwnsKeys>::erase(const char *_key)
		{
			size_t index = findIndex(_key);
			if (index != (size_t)-1) {
				if (OwnsKeys)
					Dealloc(m_keys[index]);
				m_keys[index] = (const char*)-1;
				m_slotsFree++;
				return true;
			}
			return false;
		}

		template <class Data, bool OwnsKeys>
		void HashTable<Data, OwnsKeys>::empty()
		{
			for (size_t i = 0; i < m_size; ++i) {
				if (m_keys[i] == (const char*)-1) m_keys[i] = nullptr;
				if (OwnsKeys)
					Dealloc(m_keys[i]);
			}

			memset(m_keys, 0, sizeof(const char *) * m_size);
			memset(m_data, 0, sizeof(Data) * m_size);
		}

		template <class Data, bool OwnsKeys>
		size_t HashTable<Data, OwnsKeys>::insert(const char *_key, Data const &_data)
		{
			if (m_slotsFree * 2 <= m_size) {
				grow();
			}

			size_t index = findInsertIndex(_key);
			CoreAssert(m_keys[index] == nullptr || m_keys[index] == (char*)-1);
			if (OwnsKeys)
				m_keys[index] = (const char *)Duplicate(_key);
			else
				m_keys[index] = _key;
			m_data[index] = _data;
			m_slotsFree--;

			return index;
		}

		template <class T, bool OwnsKeys>
		void SortingHashTable<T, OwnsKeys>::grow()
		{
			unsigned int oldSize = this->m_size;
			this->m_size *= 2;
			this->m_mask = (this->m_mask << 1) + 1;

			/* Copy pointers to existing data */
			const char **oldKeys = this->m_keys;
			T           *oldData = this->m_data;
			size_t      *oldOrderedIndices = m_orderedIndices;

			/* Make new data */
			this->m_keys = new const char *[this->m_size];
			this->m_data = new T [this->m_size];
			m_orderedIndices = new size_t[this->m_size];

			/* Set all new data to zero */
			memset(this->m_keys, 0, sizeof(const char *) * this->m_size);
			memset(this->m_data, 0, sizeof(T) * this->m_size);
			for (size_t i = 0; i < this->m_size; i++)
				m_orderedIndices[i] = (size_t)-1;

			/* */
			/* Go through the existing ordered index list, inserting elements into the */
			/* new table as we go */

			size_t oldI = m_firstOrderedIndex;
			size_t newI = this->findInsertIndex(oldKeys[oldI]);
			m_firstOrderedIndex = newI;
			while (oldI != (size_t)-1)
			{
				size_t nextOldI = oldOrderedIndices[oldI];

				this->m_keys[newI] = oldKeys[oldI];
				this->m_data[newI] = oldData[oldI];

				size_t nextNewI = (nextOldI != (size_t)-1) ? this->findInsertIndex(oldKeys[nextOldI]) : (size_t)-1;

				m_orderedIndices[newI] = nextNewI;

				oldI = nextOldI;
				newI = nextNewI;
			}

			this->m_slotsFree += this->m_size - oldSize;

			delete [] oldKeys;
			delete [] oldData;
			delete [] oldOrderedIndices;
		}

		/* See header for description */
		template <class T, bool OwnsKeys>
		size_t SortingHashTable<T, OwnsKeys>::findPrevKey(const char *_key) const
		{
			size_t prevI = -1;
			size_t i = m_firstOrderedIndex;

			while (1)
			{
				if (i == (size_t)-1) return prevI;

				if (Compare(this->m_keys[i], _key) >= 0)
					return prevI;

				prevI = i;
				i = m_orderedIndices[i];
			}
		}

		template <class T, bool OwnsKeys>
		SortingHashTable<T, OwnsKeys>::SortingHashTable() :
			HashTable<T, OwnsKeys>(),
			m_firstOrderedIndex(-1)
		{
			m_orderedIndices = new size_t[this->m_size];
			memset(m_orderedIndices, 0, sizeof(size_t) * this->m_size);
		}

		template <class T, bool OwnsKeys>
		SortingHashTable<T, OwnsKeys>::~SortingHashTable()
		{
			delete [] m_orderedIndices;
		}

		template <class T, bool OwnsKeys>
		size_t SortingHashTable<T, OwnsKeys>::insert(char const *_key, T const &_data)
		{
			/* */
			/* Make sure the table is big enough */

			if (this->m_slotsFree * 2 <= this->m_size) {
				grow();
			}

			/* */
			/* Do the main insert */

			size_t index = HashTable<T, OwnsKeys>::findInsertIndex(_key);
			CoreAssert(this->m_keys[index] == nullptr || this->m_keys[index] == (char*)-1);
			if (OwnsKeys)
				this->m_keys[index] = (char *)Duplicate(_key);
			else
				this->m_keys[index] = _key;
			this->m_data[index] = _data;
			this->m_slotsFree--;

			/* */
			/* Insert us into the alphabetically order index list */

			size_t i = findPrevKey(_key);
			if (i == (size_t)-1) {
				/* Handle the case when the table is empty, or the new element is going */
				/* to be the new first alphabetical element */
				m_orderedIndices[index] = m_firstOrderedIndex;
				m_firstOrderedIndex = index;
			} else	{
				m_orderedIndices[index] = m_orderedIndices[i];
				m_orderedIndices[i] = index;
			}

			return index;
		}

		template <class T, bool OwnsKeys>
		bool SortingHashTable<T, OwnsKeys>::erase(char const *_key)
		{
			int index = this->GetIndex(_key);
			if (index >= 0)	{
				return erase(index);
			}
			return false;
		}

		template <class T, bool OwnsKeys>
		bool SortingHashTable<T, OwnsKeys>::erase(size_t _index)
		{
			bool retval = false;
			/* */
			/* Remove data */

			if (this->m_keys[_index] != (char *)-1) {
				retval = true;
				if (OwnsKeys)
					Dealloc(this->m_keys[_index]);
			}
			this->m_keys[_index] = (char *)-1;
			this->m_slotsFree++;

			/* */
			/* Remove from ordered list */

			short prevIndex = -1;
			short index = m_firstOrderedIndex;
			while (m_orderedIndices[index] != _index)
			{
				CoreAssert(index != -1);
				prevIndex = index;
				index = m_orderedIndices[index];
			}

			if (prevIndex == -1) {
				m_firstOrderedIndex = -1;
			} else	{
				m_orderedIndices[prevIndex] = m_orderedIndices[index];
			}
			return retval;
		}

		template <class T, bool OwnsKeys>
		size_t SortingHashTable<T, OwnsKeys>::beginOrderedWalk()
		{
			m_nextOrderedIndex = m_orderedIndices[m_firstOrderedIndex];
			return m_firstOrderedIndex;
		}

		template <class T, bool OwnsKeys>
		size_t SortingHashTable<T, OwnsKeys>::nextOrderedIndex()
		{
			size_t rv = m_nextOrderedIndex;
			if (rv != (size_t)-1)
				m_nextOrderedIndex = m_orderedIndices[m_nextOrderedIndex];
			return rv;
		}
	}
}
