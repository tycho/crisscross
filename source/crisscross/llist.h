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
#define __included_cc_llist_h

#include <crisscross/deprecate.h>

#include <crisscross/sort.h>
#include <crisscross/darray.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A linked list node. */
		template <class T> class LListNode
		{
			public:
				/*! \brief The following node in the list. */
				LListNode *m_next;

				/*! \brief The preceding node in the list. */
				LListNode *m_previous;

				/*! \brief The data contained in this node. */
				T          m_data;

				/*! \brief The default constructor. */
				LListNode();
		};

		/*! \brief A doubly-linked list implementation. */
		template <class T>
		class LList
		{
			protected:
				/*! \brief The first node. */
				LListNode <T> *m_first;

				/*! \brief The last node. */
				LListNode <T> *m_last;

				/*! \brief The last accessed node. */
				/*!
				 *  Speeds up searches and sequential access.
				 */
				mutable LListNode <T> *m_previous;  /* Used to get quick access */

				/*! \brief The last accessed index. */
				/*!
				 *  Speeds up searches and sequential access.
				 */
				mutable size_t m_previousIndex; /* for sequential reads (common) */

				/*! \brief The number of nodes in the list. */
				size_t m_numItems;

			protected:
				inline LListNode <T> *getItem(size_t index) const;

			public:

				/*! \brief The default constructor. */
				LList();

				/*! \brief The copy constructor. */
				LList(const LList <T> &);

				/*! \brief The destructor. */
				~LList();

				/*! \brief The implicit copy operator. */
				LList & operator =(const LList <T> &);

				/*! \brief Adds data at the end of the list. */
				/*!
				 * \param _newdata The data to add to the list.
				 */
				inline void insert(const T & _newdata);

				/*! \brief Adds data at the end of the list. */
				/*!
				 * \param _newdata The data to add to the list.
				 */
				void insert_back(const T & _newdata);

				/*! \brief Adds data at the start of the list. */
				/*!
				 * \param _newdata The data to add to the list.
				 */
				void insert_front(const T & _newdata);

				/*! \brief Adds data at the specified index. */
				/*!
				 * \param _newdata The data to add to the list.
				 * \param _index The index where the node should be added.
				 */
				void insert_at(const T & _newdata, size_t _index);

				/*! \brief Gets the data at the specified index. */
				/*!
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _index The index of the data to fetch.
				 * \return The data contained in the node at the index.
				 */
				inline T const &get(size_t _index) const;

				/*! \brief Gets the address of where the data at the specified index is stored. */
				/*!
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _index The index of the node to find.
				 * \return The pointer to where the data record is stored.
				 */
				inline T *getPointer(size_t _index) const;

				/*! \brief Modifies the node at the given index. */
				/*!
				 *  This does not delete the data at the node, just the node itself.
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _rec The new value for the given index.
				 * \param _index The index of the node to modify.
				 */
				void change(T const & _rec, size_t _index);

				/*! \brief Removes the node at the given index. */
				/*!
				 *  This does not delete the data at the node, just the node itself.
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _index The index of the node to delete.
				 */
				void remove(size_t _index);

				/*! \brief Removes the node at the end of the list. */
				/*!
				 *  This does not delete the data at the node, just the node itself.
				 */
				inline void removeDataAtEnd();

				/*! \brief Finds a node's index by searching for the given data. */
				/*!
				 * \param _data The data to find.
				 * \return -1 if not found, otherwise returns the index.
				 */
				size_t find(const T & _data);

				/*! \brief Indicates the size of the linked list. */
				/*!
				 * \return The size of the linked list.
				 */
				inline size_t size() const;

				/*! \brief Determines whether a given index is within the bounds of the list. */
				/*!
				 * \param _index The index to validate.
				 * \return True if the index is valid, false otherwise.
				 */
				inline bool valid(size_t _index) const;

				/*! \brief Deletes all nodes in the list, but does not free memory allocated by data. */
				void empty();

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				T & operator [](size_t _index);

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				T const & operator [](size_t _index) const;

#ifdef ENABLE_SORTS
				/*! \brief Sorts the array using the provided method. */
				/*!
				 * \param _sortMethod The method to sort with.
				 */
				void sort(CrissCross::Data::Sorter<T> *_sortMethod);

				/*! \brief Sorts the array using the provided method. */
				/*!
				 * \param _sortMethod The method to sort with.
				 */
				void sort(CrissCross::Data::Sorter<T> &_sortMethod);
#endif
				/*! \brief Returns the overhead caused by the data structure. */
				/*!
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage() const;


#if !defined(DISABLE_DEPRECATED_CODE)
				/*
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				_CC_DEPRECATE_FUNCTION(insert) inline void PutData(T const & _rec)
				{
					insert(_rec);
				};
				_CC_DEPRECATE_FUNCTION(insert_back) inline void PutDataAtStart(T const & _rec)
				{
					insert_front(_rec);
				};
				_CC_DEPRECATE_FUNCTION(insert_back) inline void PutDataAtEnd(T const & _rec)
				{
					insert_back(_rec);
				};
				_CC_DEPRECATE_FUNCTION(insert_at) inline void PutDataAtIndex(T const & _rec, size_t _index)
				{
					insert_at(_rec, _index);
				};
				_CC_DEPRECATE_FUNCTION(find) inline size_t FindData(T const & _rec)
				{
					return find(_rec);
				};
				_CC_DEPRECATE_FUNCTION(remove) inline void RemoveData(size_t _index)
				{
					remove(_index);
				};
				_CC_DEPRECATE_FUNCTION(change) void ChangeData(T const & _rec, size_t _index);
				_CC_DEPRECATE_FUNCTION(size) inline int Size() const
				{
					return (int)size();
				};
				_CC_DEPRECATE_FUNCTION_N inline void EmptyAndDelete()
				{
					while (valid(0)) {
						delete get(0); remove(0);
					}
				};
				_CC_DEPRECATE_FUNCTION_N inline void EmptyAndDeleteArray()
				{
					while (valid(0)) {
						delete [] get(0); remove(0);
					}
				};
				_CC_DEPRECATE_FUNCTION_N inline void Empty()
				{
					empty();
				};
				_CC_DEPRECATE_FUNCTION(get) inline T getData(size_t _index) const
				{
					return get(_index);
				};
				_CC_DEPRECATE_FUNCTION(get) inline T GetData(size_t _index) const
				{
					return get(_index);
				};
				_CC_DEPRECATE_FUNCTION(valid) inline bool validIndex(size_t _index) const
				{
					return valid(_index);
				};
				_CC_DEPRECATE_FUNCTION(valid) inline bool ValidIndex(size_t _index) const
				{
					return valid(_index);
				};
				/*! @endcond */
#endif
		};
	}
}
#include <crisscross/llist.cpp>

#endif
