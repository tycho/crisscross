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
#define __included_cc_hashtable_h

#include <stdint.h>
#include <set>
#include <cstring>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A hilariously stupid HashTable implementation. */
		template <class Data>
		class HashTable
		{
		protected:
			struct kvpair_t
			{
				const char *key;
				Data data;
			};
			kvpair_t *m_data;
			uint64_t  m_size;
			uint64_t  m_slotsFree;

			uint64_t findInsertIndex(const char *_key) const;
			uint64_t findIndex(const char *_key) const;
			virtual void   grow();

		public:
			/*! \brief The constructor. */
			HashTable(uint64_t _ignored = 32);
			virtual ~HashTable();

			/*! \brief Inserts data into the table. */
			/*!
			 * \param _key The key of the data.
			 * \param _data The data to insert.
			 * \return Index of element in the table. 
			 */
			virtual uint64_t insert(const char *_key, Data const &_data);

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
			bool erase(uint64_t _index);

			/*! \brief Tests whether a key is in the table or not. */
			/*!
			 * \param _key The key of the node to find.
			 * \return True if the key is in the table, false if not.
			 */
			bool exists(const char *_key) const;

			/*! \brief Empties the table completely. */
			void empty();

			/*! \brief Indicates the maximum number of elements. */
			/*!
			 * \return Maximum number of elements the table can contain.
			 */
			inline uint64_t size() const
			{
				return m_size;
			}

			/*! \brief Indicates the number of items in the table. */
			/*!
			 * \return Number of items in the table.
			 */
			inline uint64_t used() const
			{
				return m_size - m_slotsFree;
			}
		};
	}
}

#include <crisscross/hashtable.cpp>

#endif
