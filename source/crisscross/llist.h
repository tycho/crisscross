/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_llist_h
#define __included_cc_llist_h

#include <cstdint>

#include <crisscross/cc_attr.h>

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
				T m_data;

				/*! \brief The default constructor. */
				LListNode();
		};

		/*! \brief A doubly-linked list implementation. */
		template <class T>
		class LList
		{
			public:
				/*! \brief STL-compatible and range-loop compatible iterator */
				struct LListIterator
				{
					friend class LList<T>;
					protected:
						LList<T> *m_list;
						uint32_t m_idx;

					public:
						using iterator_category = std::random_access_iterator_tag;
						using difference_type = std::ptrdiff_t;
						using value_type = T;
						using pointer = T *;
						using reference = T &;

						cc_forceinline explicit LListIterator(LList<T> *_list)
						{
							this->m_list = _list;
							this->m_idx = 0;
						}

						cc_forceinline reference operator*() const
						{
							return (*this->m_list)[this->m_idx];
						}

						cc_forceinline pointer operator->()
						{
							return &(*this->m_list)[this->m_idx];
						}

						cc_forceinline LListIterator &operator++()
						{
							this->m_idx++;
							return *this;
						}

						cc_forceinline LListIterator &operator--()
						{
							this->m_idx--;
							return *this;
						}

						cc_forceinline LListIterator operator+(const difference_type &_difference) const
						{
							LListIterator iter(m_list);
							iter.m_idx = m_idx + _difference;
							return iter;
						}

						cc_forceinline LListIterator operator-(const difference_type &_difference) const
						{
							LListIterator iter(m_list);
							iter.m_idx = m_idx - _difference;
							return iter;
						}

						cc_forceinline difference_type operator-(LListIterator const &_iter) const
						{
							return m_idx - _iter.m_idx;
						}

						cc_forceinline bool operator<(LListIterator const &_other) const
						{
							return m_idx < _other.m_idx;
						}

						cc_forceinline bool operator>(LListIterator const &_other) const
						{
							return m_idx > _other.m_idx;
						}

						inline bool operator ==(const LListIterator &_rhs) const
						{
							return this->m_idx == _rhs.m_idx && this->m_list == _rhs.m_list;
						}
						inline bool operator !=(const LListIterator &_rhs) const
						{
							return !(*this == _rhs);
						}
				};

				struct LListConstIterator
				{
					friend class LList<T>;
					protected:
						const LList<T> *m_list;
						uint32_t m_idx;

					public:
						using iterator_category = std::random_access_iterator_tag;
						using difference_type = std::ptrdiff_t;
						using value_type = T const;
						using pointer = T const *;
						using reference = T const &;

						cc_forceinline explicit LListConstIterator(const LList<T> *_list)
						{
							this->m_list = _list;
							this->m_idx = 0;
						}

						cc_forceinline reference operator*() const
						{
							return (*this->m_list)[this->m_idx];
						}

						cc_forceinline pointer operator->()
						{
							return (*this->m_list)[this->m_idx];
						}

						cc_forceinline LListConstIterator &operator++()
						{
							this->m_idx++;
							return *this;
						}

						cc_forceinline LListIterator &operator--()
						{
							this->m_idx--;
							return *this;
						}

						cc_forceinline LListConstIterator operator+(const difference_type &_difference) const
						{
							LListConstIterator iter(m_list);
							iter.m_idx = m_idx + _difference;
							return iter;
						}

						cc_forceinline LListConstIterator operator-(const difference_type &_difference) const
						{
							LListConstIterator iter(m_list);
							iter.m_idx = m_idx - _difference;
							return iter;
						}

						cc_forceinline difference_type operator-(LListConstIterator const &_iter) const
						{
							return m_idx - _iter.m_idx;
						}

						cc_forceinline bool operator<(LListConstIterator const &_other) const
						{
							return m_idx < _other.m_idx;
						}

						cc_forceinline bool operator>(LListConstIterator const &_other) const
						{
							return m_idx > _other.m_idx;
						}

						inline bool operator ==(const LListConstIterator &_rhs) const
						{
							return this->m_idx == _rhs.m_idx && this->m_list == _rhs.m_list;
						}
						inline bool operator !=(const LListConstIterator &_rhs) const
						{
							return !(*this == _rhs);
						}
				};

				using iterator = LListIterator;
				using const_iterator = LListConstIterator;

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
				mutable uint32_t m_previousIndex; /* for sequential reads (common) */

				/*! \brief The number of nodes in the list. */
				uint32_t m_numItems;

			protected:
				inline LListNode <T> *getItem(uint32_t index) const;

			public:

				/*! \brief The default constructor. */
				LList();

				/*! \brief The copy constructor. */
				LList(const LList <T> &);

				/*! \brief The destructor. */
				~LList();

				/*! \brief The implicit copy operator. */
				LList & operator =(const LList <T> &);

				/*! \brief Adds a new node to the end of the list and returns a reference to the new data element. */
				/*!
				 * \return Reference to new node's value.
				 */
				_CC_WARN_UNUSED_RESULT T &insert_new();

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
				void insert_at(const T & _newdata, uint32_t _index);

				/*! \brief Gets the data at the specified index. */
				/*!
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _index The index of the data to fetch.
				 * \param _default The default value to return if the index couldn't be retrieved.
				 * \return The data contained in the node at the index or _default if not found.
				 */
				inline T const &get(uint32_t _index, T const &_default = nullptr) const;

				/*! \brief Gets the address of where the data at the specified index is stored. */
				/*!
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _index The index of the node to find.
				 * \return The pointer to where the data record is stored.
				 */
				inline T *getPointer(uint32_t _index) const;

				/*! \brief Modifies the node at the given index. */
				/*!
				 *  This does not delete the data at the node, just the node itself.
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _rec The new value for the given index.
				 * \param _index The index of the node to modify.
				 */
				void change(T const & _rec, uint32_t _index);

				/*! \brief Removes the node at the given index. */
				/*!
				 *  This does not delete the data at the node, just the node itself.
				 * \warning Slow unless you're sequentially iterating through.
				 * \param _index The index of the node to delete.
				 */
				void remove(uint32_t _index);

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
				uint32_t find(const T & _data);

				/*! \brief Indicates the size of the linked list. */
				/*!
				 * \return The size of the linked list.
				 */
				inline uint32_t size() const;

				/*! \brief Determines whether a given index is within the bounds of the list. */
				/*!
				 * \param _index The index to validate.
				 * \return True if the index is valid, false otherwise.
				 */
				inline bool valid(uint32_t _index) const;

				/*! \brief Deletes all nodes in the list, but does not free memory allocated by data. */
				void empty();

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				T & operator [](uint32_t _index);

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				T const & operator [](uint32_t _index) const;

				/*! \brief Returns the overhead caused by the data structure. */
				/*!
				 * \return Memory usage in bytes.
				 */
				uint32_t mem_usage() const;

				iterator begin()
				{
					iterator it(this);
					while (!valid(it.m_idx))
						it.m_idx++;
					return it;
				}

				iterator end()
				{
					iterator it(this);
					it.m_idx = m_numItems;
					return it;
				}

				const_iterator begin() const
				{
					const_iterator it(this);
					while (!valid(it.m_idx))
						it.m_idx++;
					return it;
				}

				const_iterator end() const
				{
					const_iterator it(this);
					it.m_idx = m_numItems;
					return it;
				}

#if !defined (DISABLE_DEPRECATED_CODE)
				/*
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				_CC_DEPRECATE_FUNCTION(insert) inline void PutData(T const & _rec)
				{
					insert(_rec);
				}
				_CC_DEPRECATE_FUNCTION(insert_back) inline void PutDataAtStart(T const & _rec)
				{
					insert_front(_rec);
				}
				_CC_DEPRECATE_FUNCTION(insert_back) inline void PutDataAtEnd(T const & _rec)
				{
					insert_back(_rec);
				}
				_CC_DEPRECATE_FUNCTION(insert_at) inline void PutDataAtIndex(T const & _rec, uint32_t _index)
				{
					insert_at(_rec, _index);
				}
				_CC_DEPRECATE_FUNCTION(find) inline uint32_t FindData(T const & _rec)
				{
					return find(_rec);
				}
				_CC_DEPRECATE_FUNCTION(remove) inline void RemoveData(uint32_t _index)
				{
					remove(_index);
				}
				_CC_DEPRECATE_FUNCTION(change) void ChangeData(T const & _rec, uint32_t _index);
				_CC_DEPRECATE_FUNCTION(size) inline int Size() const
				{
					return (int)size();
				}
				_CC_DEPRECATE_FUNCTION_N inline void EmptyAndDelete()
				{
					static_assert(std::is_pointer<T>::value, "attempting to delete a non-pointer type");
					while (valid(0)) {
						delete get(0); remove(0);
					}
				}
				_CC_DEPRECATE_FUNCTION_N inline void EmptyAndDeleteArray()
				{
					static_assert(std::is_pointer<T>::value, "attempting to delete a non-pointer type");
					while (valid(0)) {
						delete [] get(0); remove(0);
					}
				}
				_CC_DEPRECATE_FUNCTION_N inline void Empty()
				{
					empty();
				}
				_CC_DEPRECATE_FUNCTION(get) inline T getData(uint32_t _index) const
				{
					return get(_index);
				}
				_CC_DEPRECATE_FUNCTION(get) inline T GetData(uint32_t _index) const
				{
					return get(_index);
				}
				_CC_DEPRECATE_FUNCTION(valid) inline bool validIndex(uint32_t _index) const
				{
					return valid(_index);
				}
				_CC_DEPRECATE_FUNCTION(valid) inline bool ValidIndex(uint32_t _index) const
				{
					return valid(_index);
				}
				/*! @endcond */
#endif
		};
	}
}
#include <crisscross/llist.cpp>

#endif
