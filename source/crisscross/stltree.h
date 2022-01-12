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

#ifndef __included_cc_stltree_h
#define __included_cc_stltree_h

#include <crisscross/platform_detect.h>

#define ENABLE_STLTREE

#if defined (TARGET_COMPILER_BORLAND)
#undef ENABLE_STLTREE
#endif

/*
 * Something is wrong with Microsoft's and
 * Borland's std::map implementation.
 */
#ifdef ENABLE_STLTREE

#include <crisscross/cc_attr.h>
#include <crisscross/compare.h>
#include <crisscross/internal_mem.h>
#include <crisscross/darray.h>
#include <crisscross/node.h>

#include <map>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief An STL map wrapper. */
		/*!
		 *      This is a tree which does NOT allow duplicate keys.
		 */
		template <class Key, class Data, bool OwnsKeys = true>
		class STLTree
		{
			private:
				std::map<Key,Data,CrissCross::Data::LessThanComparator<Key> > m_map;

			public:

				/*! \brief The default constructor. */
				STLTree() {}

				/*! \brief The destructor. */
				~STLTree() { empty(); }

				/*! \brief Empties the entire tree. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed
				 *    separately. The preferred way to do this is to serialize the data into a DArray
				 *    with ConvertToDArray() and then iterate through it to delete the data in whatever
				 *    way is proper.
				 */
				cc_forceinline void empty()
				{
					if (OwnsKeys)
						for (auto iter : m_map) {
							Dealloc(iter.first);
						}
					m_map.clear();
				}

				/*! \brief Inserts data into the tree. */
				/*!
				 * \param _key The key of the data.
				 * \param _rec The data to insert.
				 * \return True on success, false on failure.
				 */
				cc_forceinline bool insert(Key const &_key, Data const &_rec)
				{
					if (exists(_key))
						return false;
					if (OwnsKeys)
						m_map[Duplicate(_key)] = _rec;
					else
						m_map[_key] = _rec;
					return true;
				}

				/*! \brief Tests whether a key is in the tree or not. */
				/*!
				 * \param _key The key of the node to find.
				 * \return True if the key is in the tree, false if not.
				 */
				cc_forceinline bool exists(Key const &_key) const
				{ return m_map.count(_key) != 0; }

				/*! \brief Change the data at the given node. */
				/*!
				 * \param _key The key of the node to be modified.
				 * \param _rec The data to insert.
				 * \return True on success, false on failure.
				 */
				cc_forceinline bool replace(Key const &_key, Data const &_rec)
				{
					if (!exists(_key))
						return false;
					m_map[_key] = _rec;
					return true;
				}

				/*! \brief Finds a node in the tree and returns the data at that node. */
				/*!
				 * \param _key The key of the node to find.
				 * \param _default The value to return if the item couldn't be found.
				 * \return If found, returns the data at the node, otherwise _default is returned.
				 */
				template <class TypedData = Data>
				cc_forceinline TypedData find(Key const &_key, TypedData const &_default = nullptr) const
				{
					auto iter = m_map.find(_key);
					if (iter == m_map.cend())
						return _default;
					return (TypedData)(iter->second);
				}

				/*! \brief Deletes a node from the tree, specified by the node's key. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
				 * \param _key The key of the node to delete.
				 * \return True on success, false on failure
				 */
				cc_forceinline bool erase(Key const &_key)
				{
					auto iter = m_map.find(_key);
					if (iter == m_map.end())
						return false;
					Key key = iter->first;
					m_map.erase(iter);
					if (OwnsKeys)
						Dealloc(key);
					return true;
				}

				/*! \brief Indicates the size of the tree. */
				/*!
				 * \return Size of the tree.
				 */
				cc_forceinline size_t size() const
				{ return m_map.size(); }

				/*! \brief Converts the tree data into a linearized DArray. */
				/*!
				 * \return A DArray containing the data of the tree.
				 * \warning Delete the returned DArray when done with it.
				 */
				template <class TypedData>
				cc_forceinline DArray <TypedData> *ConvertToDArray() const
				{
					DArray<TypedData> *darray = new DArray<TypedData>(size());
					typename std::map<Key,Data,CrissCross::Data::LessThanComparator<Key> >::const_iterator iter;
					for (iter = m_map.begin(); iter != m_map.end(); iter++) {
						darray->insert((TypedData)(iter->second));
					}
					return darray;
				}

				/*! \brief Converts the tree keys into a linearized DArray. */
				/*!
				 * \return A DArray containing the keys in the tree.
				 * \warning Delete the returned DArray when done with it.
				 */
				cc_forceinline DArray <Key> *ConvertIndexToDArray() const
				{
					DArray<Key> *darray = new DArray<Key>(size());
					typename std::map<Key,Data,CrissCross::Data::LessThanComparator<Key> >::const_iterator iter;
					for (iter = m_map.begin(); iter != m_map.end(); iter++) {
						darray->insert(iter->first);
					}
					return darray;
				}

#if !defined (DISABLE_DEPRECATED_CODE)
				/*
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				_CC_DEPRECATE_FUNCTION(insert) cc_forceinline void PutData(Key const &_key, Data const & _rec)
				{
					insert(_key, _rec);
				}
				_CC_DEPRECATE_FUNCTION(find)   cc_forceinline Data GetData(Key const &_key, Data const &_default = nullptr)
				{
					return find(_key, _default);
				}
				_CC_DEPRECATE_FUNCTION(erase)  cc_forceinline void RemoveData(Key const &_key)
				{
					erase(_key);
				}
				_CC_DEPRECATE_FUNCTION(erase)  cc_forceinline void RemoveData(Key const &_key, Data const & _rec)
				{
					erase(_key, _rec);
				}
				_CC_DEPRECATE_FUNCTION(size)   cc_forceinline int  Size() const
				{
					return (int)size();
				}
				_CC_DEPRECATE_FUNCTION(empty)  cc_forceinline void Empty()
				{
					empty();
				}
				/*! @endcond */
#endif
		};
	}
}

#endif
#endif
