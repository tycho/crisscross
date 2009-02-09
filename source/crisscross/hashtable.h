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

#include <crisscross/avltree.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A simple HashTable. */
		template <class Data>
		class HashTable
		{
			private:
				/*! \brief Private copy constructor. */
				/*!
				 * Not yet written, so declared private for now to prevent an auto-generated
				 * one from performing an unexpected operation.
				 */
				HashTable(const HashTable<Data> &);

				/*! \brief Private assignment operator. */
				/*!
				 * Not yet written, so declared private for now to prevent an auto-generated
				 * one from performing an unexpected operation.
				 */
				HashTable<Data> &operator =(const HashTable<Data> &);

			protected:
				char       **m_keys;
				Data        *m_data;
				unsigned int m_slotsFree;
				unsigned int m_size;
				unsigned int m_mask;

				size_t findInsertIndex(const char *_key) const;
				size_t findIndex(const char *_key) const;
				void   grow();

			public:
				/*! \brief The constructor. */
				/*!
				 * \param _initialSize The initial size of the hash table. Minimum is 32.
				 */
				HashTable(size_t _initialSize = 32);
				~HashTable();

				/*! \brief Inserts data into the table. */
				/*!
				 * \param _key The key of the data.
				 * \param _data The data to insert.
				 * \return True on success, false on failure.
				 */
				bool insert(const char *_key, Data const &_data);

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
				};

				/*! \brief Indicates the number of items in the table. */
				/*!
				 * \return Number of items in the table.
				 */
				inline size_t used() const
				{
					return m_size - m_slotsFree;
				};

#if !defined (DISABLE_DEPRECATED_CODE)
				/*
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				int GetIndex(const char *_key) const
				{
					return findIndex(_key);
				};
				int PutData(const char *_key, Data const &_data)
				{
					return insert(_key, _data);
				};
				Data GetData(const char *_key, Data const &_default = NULL) const
				{
					return find(_key, _default);
				};
				Data GetData(unsigned int _index) const
				{
					return m_data[_index];
				};
				Data *GetPointer(const char *_key) const
				{
					int index = GetIndex(_key);
					if (index >= 0)	{
						return &m_data[index];
					}
					return NULL;
				};
				Data *GetPointer(unsigned int _index) const
				{
					return &m_data[_index];
				};
				void RemoveData(const char *_key)
				{
					erase(_key);
				};
				void RemoveData(unsigned int _index)
				{
				};
				#if 0
				void Empty();
				void EmptyAndDelete();
				bool ValidIndex(unsigned int _x) const;
				unsigned int Size() const;                                                           /* Returns total table size, NOT number of slots used */
				unsigned int NumUsed() const;
				Data operator [](unsigned int _index) const;
				char const *GetName(unsigned int _index) const;
				#endif
				/*! @endcond */
#endif
		};
	}
}

#include <crisscross/hashtable.cpp>

#endif
