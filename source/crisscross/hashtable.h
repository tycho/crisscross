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
#define __included_cc_hashtable_h

#include <crisscross/avltree.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A simple HashTable. */
		template <class Key, class Data>
		class HashTable
		{
			private:
				/*! \brief Private copy constructor. */
				/*!
				 * Not yet written, so declared private for now to prevent an auto-generated
				 * one from performing an unexpected operation.
				 */
				HashTable(const HashTable<Key, Data> &);

				/*! \brief Private assignment operator. */
				/*!
				 * Not yet written, so declared private for now to prevent an auto-generated
				 * one from performing an unexpected operation.
				 */
				HashTable<Key, Data> &operator =(const HashTable<Key, Data> &);

			protected:
				typedef AVLTree<Key, Data> tree_t;

				tree_t     * *m_array;
				unsigned long m_size;
				unsigned long m_used;

				size_t findIndex(Key const &_key) const;

			public:
				/*! \brief The constructor. */
				/*!
				 * \param _initialSize The initial size of the hash table. Minimum is 500.
				 */
				HashTable(size_t _initialSize = 25);
				~HashTable();

				/*! \brief Inserts data into the table. */
				/*!
				 * \param _key The key of the data.
				 * \param _data The data to insert.
				 * \return True on success, false on failure.
				 */
				bool insert(Key const &_key, Data const &_data);

				/*! \brief Finds a node in the table and copies the data from that node to a specified location. */
				/*!
				 * \param _key The key of the node to find.
				 * \param _data On return, will contain the data at the node. If not found, _data does not change.
				 * \return True on success, false on failure.
				 */
				bool find(Key const &_key, Data &_data) const;

				/*! \brief Finds a node in the table and returns the data at that node. */
				/*!
				 * \param _key The key of the item to find.
				 * \return The data at the node. NULL if not found.
				 * \deprecated The return value of this function could be unpredictable if the
				 *                          contents of the table was anything but pointers or integers.
				 * \sa find
				 */
				Data find(Key const & _key) const;

				/*! \brief Deletes a node from the table, specified by the node's key. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
				 * \param _key The key of the node to delete.
				 * \return True on success, false on failure
				 */
				bool erase(Key const &_key);

				/*! \brief Tests whether a key is in the table or not. */
				/*!
				 * \param _key The key of the node to find.
				 * \return True if the key is in the tree, false if not.
				 */
				bool exists(Key const &_key) const;

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
					return m_used;
				};

				/*! \brief Returns the overhead caused by the data structure. */
				/*!
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage() const;
		};
	}
}

#include <crisscross/hashtable.cpp>

#endif
