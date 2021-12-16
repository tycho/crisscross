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

#ifndef __included_cc_darray_h
#define __included_cc_darray_h

#include <cstring>
#include <algorithm>
#include <vector>

#include <crisscross/cc_attr.h>
#include <crisscross/debug.h>
#include <crisscross/dstack.h>
#include <crisscross/sort.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A dynamic array implementation. */
		template <class T> class DArray
		{
			public:
				/*! \brief STL-compatible and range-loop compatible iterator */
				struct DArrayIterator
				{
					friend class DArray<T>;
				protected:
					DArray<T> *m_darray;
					size_t m_idx;

					cc_forceinline void ensure_valid()
					{
						// Keep incrementing the index until we find a filled slot or the end of the array.
						while (!this->m_darray->valid(this->m_idx) && this->m_idx != this->m_darray->m_arraySize)
							this->m_idx++;
					}

					cc_forceinline void advance()
					{
						this->m_idx++;
						ensure_valid();
					}

				public:
					using iterator_category = std::forward_iterator_tag;
					using difference_type = std::ptrdiff_t;
					using value_type = T;
					using pointer = T*;
					using reference = std::pair<std::size_t, T &>;

					cc_forceinline explicit DArrayIterator(DArray<T> *_darray) {
						this->m_darray = _darray;
						this->m_idx = 0;
					}

					cc_forceinline reference operator*() const {
						return reference(this->m_idx, this->m_darray->m_array[this->m_idx]);
					}

					cc_forceinline pointer operator->() {
						return &this->m_darray->m_array[this->m_idx];
					}

					cc_forceinline DArrayIterator &operator++()
					{
						advance();
						return *this;
					}

					inline bool operator == (const DArrayIterator &_rhs) const
					{
						return this->m_idx == _rhs.m_idx && this->m_darray == _rhs.m_darray;
					}
					inline bool operator != (const DArrayIterator &_rhs) const
					{
						return !(*this == _rhs);
					}
				};

				struct DArrayConstIterator
				{
					friend class DArray<T>;
				protected:
					const DArray<T> *m_darray;
					size_t m_idx;

					cc_forceinline void ensure_valid()
					{
						// Keep incrementing the index until we find a filled slot or the end of the array.
						while (!this->m_darray->valid(this->m_idx) && this->m_idx != this->m_darray->m_arraySize)
							this->m_idx++;
					}

					cc_forceinline void advance()
					{
						this->m_idx++;
						ensure_valid();
					}

				public:
					using iterator_category = std::forward_iterator_tag;
					using difference_type = std::ptrdiff_t;
					using value_type = T const;
					using pointer = T const *;
					using reference = std::pair<std::size_t, T const &>;

					cc_forceinline explicit DArrayConstIterator(const DArray<T> *_darray) {
						this->m_darray = _darray;
						this->m_idx = 0;
					}

					cc_forceinline reference operator*() const {
						return reference(this->m_idx, this->m_darray->m_array[this->m_idx]);
					}

					cc_forceinline pointer operator->() {
						return &this->m_darray->m_array[this->m_idx];
					}

					cc_forceinline DArrayConstIterator &operator++()
					{
						advance();
						return *this;
					}

					inline bool operator == (const DArrayConstIterator &_rhs) const
					{
						return this->m_idx == _rhs.m_idx && this->m_darray == _rhs.m_darray;
					}
					inline bool operator != (const DArrayConstIterator &_rhs) const
					{
						return !(*this == _rhs);
					}
				};

				using iterator = DArrayIterator;
				using const_iterator = DArrayConstIterator;

			protected:
				/*! \brief The size by which to increase the size of the array when there are no more empty nodes. */
				/*!
				 * If set to -1, it will double the size of the array each time the array grows.
				 * \sa setStepSize
				 * \sa setStepDouble
				 */
				int m_stepSize;

				/*! \brief The next (possible) linear insertion position. Might
				 *  be occupied, in which case we search for an alternative.
				 */
				size_t m_nextInsertPos;

				/*! \brief The current size of the array. */
				/*!
				 * \sa setSize
				 */
				size_t m_arraySize;

				/*! \brief The number of used items in the array. */
				size_t m_numUsed;

				/*! \brief The actual array which stores our data. */
				T *m_array;

				/*! \brief An array to indicate which nodes in m_array are in use. */
				std::vector<bool> m_shadow;

				/*! \brief Increases the size of the array. */
				void grow();

				/*! \brief Recounts the number of used nodes. */
				void recount();

				/*! \brief Gets the next empty node. */
				/*!
				 * Typically can just pop an item off the empty_nodes stack. If
				 * there are no other empty nodes remaining, then it will
				 * automatically Grow() the array.
				 * \return An index in m_array.
				 */
				size_t getNextFree();

			public:
				/*! \brief The default constructor. */
				DArray();

				/*! \brief Prevent copying DArrays implicitly */
				DArray(DArray<T> const &) = delete;

				/*! \brief The destructor. */
				~DArray();

				/*! \brief Truncates the array, if it's less than one third filled. */
				void compact();

				/*! \brief Truncates the array to the last used element. */
				void minimize();

				/*! \brief Sets the size of the array. */
				/*!
				 * \param _newsize The new array size.
				 */
				void setSize(size_t _newsize);

				/*! \brief Sets the size of the array. */
				/*!
				 * \param _newsize The new array size.
				 */
				void setMinimumSize(size_t _newsize);

				/*! \brief Sets the size of the array, and marks all the slots
				 * as "in use". This is useful for pre-allocating large chunks
				 * of memory for direct access. */
				/*!
				 * \param _newsize The new array size.
				 */
				void reserve(size_t _newsize);

				/*! \brief Sets the step size used in Grow(). */
				/*!
				 * Parameter _newStepSize should be larger than 1. A step size of 1 forces
				 * the DArray to resize way too often. A step size of -1 is a magic value
				 * which makes the DArray double in size on each grow() call (offers a pretty
				 * good speedup).
				 * \param _newstepsize The step size to use in grow().
				 */
				void setStepSize(int _newstepsize);

				/*! \brief Sets the step size to double the array size when a Grow() is necessitated. */
				void setStepDouble();

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				inline T get(size_t _index) const;

				/*! \brief Removes the data at the given index. */
				/*!
				 * \param _index The index of the node to clear.
				 */
				void remove(size_t _index);

				/*! \brief Finds the data in the array. */
				/*!
				 *  A return value of -1 means the data couldn't be found.
				 * \param _data The data to find.
				 * \return The index where the given data is located.
				 */
				size_t find(T const & _data) const;

				/*! \brief Constructs a new instance of T in an empty slot and returns its new index. */
				/*!
				 * \return The index of the node where the data was stored.
				 */
				size_t allocate();

				/*! \brief Inserts data into the array at the first available index. */
				/*!
				 * \param _newdata The data to put into the array.
				 * \return The index of the node where the data was stored.
				 */
				size_t insert(T const & _newdata);

				/*! \brief Inserts data into the array at the given index. */
				/*!
				 * \param _newdata The data to put into the array.
				 * \param _index The index in the array where the data should
				 *      be put, regardless of existing contents.
				 */
				void insert(T const & _newdata, size_t _index);

				/*! \brief Indicates the number of used nodes. */
				/*!
				 * \return The number of used nodes.
				 */
				size_t used() const
				{
					return m_numUsed;
				}

				/*! \brief Indicates the total size of the array. */
				/*!
				 * \return The size of the array.
				 */
				size_t size() const
				{
					return m_arraySize;
				}

				/*! \brief Indicates whether a given index is valid. */
				/*!
				 *  Tests whether the index is within the bounds of the array and
				 *  is not an empty node.
				 * \param _index The index to test.
				 * \return Boolean value. True if valid, false if not.
				 */
				bool valid(size_t _index) const
				{
					return (_index < m_arraySize && m_shadow[_index]);
				}

				/*! \brief Empties the array but does NOT free any pointers stored in the array. */
				/*!
				 *  The array must be iterated through and any pointers must be freed manually before calling this.
				 */
				void empty(bool _freeMemory = true);

				/*! \brief Sorts the array using the provided method. */
				/*!
				 * \param _sortMethod The method to sort with.
				 * \return The number of assignments and comparisons to finish the sort.
				 */
				int sort(CrissCross::Data::Sorter<T> *_sortMethod);

				/*! \brief Sorts the array using the provided method. */
				/*!
				 * \param _sortMethod The method to sort with.
				 * \return The number of assignments and comparisons to finish the sort.
				 */
				int sort(CrissCross::Data::Sorter<T> &_sortMethod);

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				__inline T & operator [](size_t _index);

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				__inline T const & operator [](size_t _index) const;

				/*! \brief Returns the overhead caused by the data structure. */
				/*!
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage() const;

				/*! \brief Empties the array and deletes the data contained in it with the 'delete' operator. */
				inline void flush();

				/*! \brief Empties the array and deletes the data contained in it with the 'delete []' operator. */
				inline void flushArray();

				iterator begin() {
					iterator it(this);
					it.ensure_valid();
					return it;
				}

				iterator end() {
					iterator it(this);
					it.m_idx = m_arraySize;
					return it;
				}

				const_iterator begin() const {
					const_iterator it(this);
					it.ensure_valid();
					return it;
				}

				const_iterator end() const {
					const_iterator it(this);
					it.m_idx = m_arraySize;
					return it;
				}

#if !defined (DISABLE_DEPRECATED_CODE)
				/*
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				_CC_DEPRECATE_FUNCTION(insert) inline int PutData(T const & _rec)
				{
					return (int)insert(_rec);
				}
				_CC_DEPRECATE_FUNCTION(insert) inline void PutData(T const & _rec, size_t _index)
				{
					insert(_rec, _index);
				}
				_CC_DEPRECATE_FUNCTION(remove) inline void RemoveData(size_t _index)
				{
					remove(_index);
				}
				_CC_DEPRECATE_FUNCTION_N void ChangeData(T const & _rec, size_t _index);
				_CC_DEPRECATE_FUNCTION(size) inline int Size() const
				{
					return (int)size();
				}
				_CC_DEPRECATE_FUNCTION(valid) inline bool validIndex(size_t _index) const
				{
					return valid(_index);
				}
				_CC_DEPRECATE_FUNCTION(valid) inline bool ValidIndex(size_t _index) const
				{
					return valid(_index);
				}
				_CC_DEPRECATE_FUNCTION(empty) inline void Empty()
				{
					empty();
				}
				_CC_DEPRECATE_FUNCTION(flush) inline void EmptyAndDelete();
				_CC_DEPRECATE_FUNCTION(flushArray) inline void EmptyAndDeleteArray();
				_CC_DEPRECATE_FUNCTION(get) inline T getData(size_t _index) const
				{
					return get(_index);
				}
				_CC_DEPRECATE_FUNCTION(get) inline T GetData(size_t _index) const
				{
					return get(_index);
				}
				_CC_DEPRECATE_FUNCTION(used) inline int numUsed() const
				{
					return (int)used();
				}
				_CC_DEPRECATE_FUNCTION(used) inline int NumUsed() const
				{
					return (int)used();
				}
				_CC_DEPRECATE_FUNCTION(setSize) inline void SetSize(size_t _newsize)
				{
					setSize(_newsize);
				}
				_CC_DEPRECATE_FUNCTION(setStepSize) inline void SetStepSize(int _newstepsize)
				{
					setStepSize(_newstepsize);
				}
				_CC_DEPRECATE_FUNCTION(sort) inline void Sort()
				{
					CrissCross::Data::Sorter<T> *s = new CrissCross::Data::HeapSort<T>();
					sort(s);
					delete s;
				}
				/*! @endcond */
#endif
		};
	}
}

#include <crisscross/darray.cpp>
#include <crisscross/rbtree.h>

#endif
