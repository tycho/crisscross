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

#ifndef __included_cc_darray_h
#define __included_cc_darray_h

#include <crisscross/deprecate.h>
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
			private:
				/*! \brief A DStack containing indices of empty nodes in the array. */
				/*!
				 *  Vastly speeds up insertions by keeping track of where empty spaces are.
				 */
				DStack<size_t> *m_emptyNodes;

			protected:
				/*! \brief The size by which to increase the size of the array when there are no more empty nodes. */
				/*!
				 * If set to -1, it will double the size of the array each time the array grows.
				 * \sa setStepSize
				 * \sa setStepDouble
				 */
				int             m_stepSize;

				/*! \brief The current size of the array. */
				/*!
				 * \sa setSize
				 */
				size_t          m_arraySize;

				/*! \brief The number of used items in the array. */
				size_t          m_numUsed;

				/*! \brief The actual array which stores our data. */
				T              *m_array;

				/*! \brief An array to indicate which nodes in m_array are in use. */
				char           *m_shadow;

				/*! \brief Increases the size of the array. */
				inline void grow();

				/*! \brief Rebuilds the empty node stack. */
				void rebuildStack();

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

				/*! \brief The secondary constructor. */
				/*!
				 * Parameter _newStepSize should be larger than 1. A step size of 1 forces
				 * the DArray to resize way too often. A step size of -1 is a magic value
				 * which makes the DArray double in size on each grow() call (offers a pretty
				 * good speedup).
				 * \param _newStepSize The step size to use in grow().
				 */
				DArray(int _newStepSize);

				/*! \brief The destructor. */
				~DArray();

				/*! \brief Sets the size of the array. */
				/*!
				 * \param _newsize The new array size.
				 */
				void setSize(size_t _newsize);

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
				size_t find(T const & _data);

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
				inline size_t used() const
				{
					return m_numUsed;
				};

				/*! \brief Indicates the total size of the array. */
				/*!
				 * \return The size of the array.
				 */
				inline size_t size() const
				{
					return m_arraySize;
				};

				/*! \brief Indicates whether a given index is valid. */
				/*!
				 *  Tests whether the index is within the bounds of the array and
				 *  is an empty node.
				 * \param _index The index to test.
				 * \return Boolean value. True if valid, false if not.
				 */
				inline bool valid(size_t _index) const
				{
					return (_index < m_arraySize && m_shadow[_index]);
				};

				/*! \brief Empties the array but does NOT free any pointers stored in the array. */
				/*!
				 *  The array must be iterated through and any pointers must be freed manually before calling this.
				 */
				void empty();

#ifdef ENABLE_SORTS
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
#endif

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				inline T & operator [](size_t _index);

				/*! \brief Gets the data at the given index. */
				/*!
				 * \param _index The index of the node to get data from.
				 * \return The data stored at the index.
				 */
				inline T const & operator [](size_t _index) const;

				/*! \brief Returns the overhead caused by the data structure. */
				/*!
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage() const;

				/*! \brief Empties the array and deletes the data contained in it with the 'delete' operator. */
				inline void flush();

				/*! \brief Empties the array and deletes the data contained in it with the 'delete []' operator. */
				inline void flushArray();

#if !defined(DISABLE_DEPRECATED_CODE)
				/*
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				_CC_DEPRECATE_FUNCTION(insert) inline size_t PutData(T const & _rec)
				{
					return insert(_rec);
				};
				_CC_DEPRECATE_FUNCTION(insert) inline void PutData(T const & _rec, size_t _index)
				{
					insert(_rec, _index);
				};
				_CC_DEPRECATE_FUNCTION(remove) inline void RemoveData(size_t _index)
				{
					remove(_index);
				};
				_CC_DEPRECATE_FUNCTION_N void ChangeData(T const & _rec, size_t _index);
				_CC_DEPRECATE_FUNCTION(size) inline int Size() const
				{
					return (int)size();
				};
				_CC_DEPRECATE_FUNCTION(valid) inline bool validIndex(size_t _index) const
				{
					return valid(_index);
				};
				_CC_DEPRECATE_FUNCTION(valid) inline bool ValidIndex(size_t _index) const
				{
					return valid(_index);
				};
				_CC_DEPRECATE_FUNCTION(empty) inline void Empty()
				{
					empty();
				};
				_CC_DEPRECATE_FUNCTION(flush) inline void EmptyAndDelete();
				_CC_DEPRECATE_FUNCTION(flushArray) inline void EmptyAndDeleteArray();
				_CC_DEPRECATE_FUNCTION(get) inline T getData(size_t _index) const
				{
					return get(_index);
				};
				_CC_DEPRECATE_FUNCTION(get) inline T GetData(size_t _index) const
				{
					return get(_index);
				};
				_CC_DEPRECATE_FUNCTION(used) inline size_t numUsed() const
				{
					return used();
				};
				_CC_DEPRECATE_FUNCTION(used) inline size_t NumUsed() const
				{
					return used();
				};
				_CC_DEPRECATE_FUNCTION(setSize) inline void SetSize(size_t _newsize)
				{
					setSize(_newsize);
				};
				_CC_DEPRECATE_FUNCTION(setStepSize) inline void SetStepSize(int _newstepsize)
				{
					setStepSize(_newstepsize);
				};
				_CC_DEPRECATE_FUNCTION(sort) inline void Sort()
				{
					CrissCross::Data::Sorter<T> *s = new CrissCross::Data::HeapSort<T>();
					sort(s);
					delete s;
				};
				/*! @endcond */
#endif
		};
	}
}

#include <crisscross/darray.cpp>

#endif
