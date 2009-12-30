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
#define __included_cc_hashtable_h

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A simple HashTable. */
		template <class Data>
		class HashTable
		{
		protected:
			char       **m_keys;
			Data        *m_data;
			unsigned int m_slotsFree;
			unsigned int m_size;
			unsigned int m_mask;

			size_t findInsertIndex(const char *_key) const;
			size_t findIndex(const char *_key) const;
			virtual void   grow();

		public:
			/*! \brief The constructor. */
			/*!
			 * \param _initialSize The initial size of the hash table. Minimum is 32.
			 */
			HashTable(size_t _initialSize = 32);
			virtual ~HashTable();

			/*! \brief Inserts data into the table. */
			/*!
			 * \param _key The key of the data.
			 * \param _data The data to insert.
			 * \return True on success, false on failure.
			 */
			virtual size_t insert(const char *_key, Data const &_data);

			/*! \brief Finds a node in the table and returns the data at that node. */
			/*!
			 * \param _key The key of the node to find.
			 * \param _default The value to return if the item couldn't be found.
			 * \return If found, returns the data at the node, otherwise _default is returned.
			 */
			Data find(const char *_key, Data const &_default = NULL) const;

			/*! \brief Deletes a node from the table, specified by the node's key. */
			/*!
			 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
			 * \param _key The key of the node to delete.
			 * \return True on success, false on failure
			 */
			bool erase(const char *_key);

			/*! \brief Deletes a node from the table, specified by the node's index. */
			/*!
			 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
			 * \param _index The index of the node to delete.
			 * \return True on success, false on failure
			 */
			bool erase(size_t _index);

			/*! \brief Tests whether a key is in the table or not. */
			/*!
			 * \param _key The key of the node to find.
			 * \return True if the key is in the table, false if not.
			 */
			bool exists(const char *_key) const;

			/*! \brief Empties the table completely. */
			void empty();

			/*! \brief Indicates the number of subtrees. */
			/*!
			 * \return Number of subtrees in the table.
			 */
			inline size_t size() const
			{
				return m_size;
			}

			/*! \brief Indicates the number of items in the table. */
			/*!
			 * \return Number of items in the table.
			 */
			inline size_t used() const
			{
				return m_size - m_slotsFree;
			}

			bool valid(size_t _index) const
			{
				CoreAssert(_index < m_size);
				return m_keys[_index] != NULL &&
				       m_keys[_index] != (char*)-1;
			}

			Data const &operator [](size_t _index) const
			{
				CoreAssert(_index < m_size);
				return m_data[_index];
			}

			Data &operator [](size_t _index)
			{
				CoreAssert(_index < m_size);
				return m_data[_index];
			}

#if !defined (DISABLE_DEPRECATED_CODE)
			/*
			 *      Deprecated Compatibility Functions
			 *      Provided for compatibility with Tosser I
			 */
			/*! @cond */
			int GetIndex(const char *_key) const
			{
				return (int)findIndex(_key);
			}
			int PutData(const char *_key, Data const &_data)
			{
				return (int)insert(_key, _data);
			}
			Data GetData(const char *_key, Data const &_default = NULL) const
			{
				return find(_key, _default);
			}
			Data GetData(unsigned int _index) const
			{
				return m_data[_index];
			}
			Data *GetPointer(const char *_key) const
			{
				int index = GetIndex(_key);
				if (index >= 0)	{
					return &m_data[index];
				}
				return NULL;
			}
			Data *GetPointer(unsigned int _index) const
			{
				return &m_data[_index];
			}
			void RemoveData(const char *_key)
			{
				erase(_key);
			}
			void RemoveData(unsigned int _index)
			{
			}
			unsigned int Size() const { return (int)size(); }
			unsigned int NumUsed() const { return (int)used(); }
			bool ValidIndex(unsigned int _x) const { return valid(_x); }
			char const *GetName(unsigned int _index) const { return m_keys[_index]; }
			void Empty() { empty(); }
			void EmptyAndDelete()
			{
				for (size_t i = 0; i < m_size; ++i) {
					if (m_keys[i] != NULL && m_keys[i] != (char *)-1) {
						delete m_data[i];
					}
				}
				empty();
			}
			/*! @endcond */
#endif
		};

		template <class Data>
		class SortingHashTable : public HashTable<Data>
		{
		protected:
			size_t	*m_orderedIndices;                    /* Stores a chain of indices that make it easy to walk through the table in alphabetical key order */
			size_t	m_firstOrderedIndex;                  /* The index of the alphabetically first table element */
			size_t	m_nextOrderedIndex;                   /* Used by nextOrderedIndex */

			void	grow();
			size_t	findPrevKey(char const *_key) const;  /* Returns the index of the table element whose key is alphabetically previous to the specified key */

		public:
			SortingHashTable();
			~SortingHashTable();

			size_t	insert(char const *_key, Data const &_data);
			bool	erase(char const *_key);
			bool	erase(size_t _index);

			size_t	beginOrderedWalk();
			size_t	nextOrderedIndex();
#if !defined (DISABLE_DEPRECATED_CODE)
			/*
			 *      Deprecated Compatibility Functions
			 *      Provided for compatibility with Tosser I
			 */
			/*! @cond */
			size_t	StartOrderedWalk() { return beginOrderedWalk(); }
			size_t	GetNextOrderedIndex() { return nextOrderedIndex(); }
			/*! @endcond */
#endif
		};
	}
}

#include <crisscross/hashtable.cpp>

#endif
