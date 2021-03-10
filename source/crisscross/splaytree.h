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

#ifndef __included_cc_splaytree_h
#define __included_cc_splaytree_h

#include <crisscross/cc_attr.h>
#include <crisscross/deprecate.h>
#include <crisscross/internal_mem.h>
#include <crisscross/darray.h>
#include <crisscross/node.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A splay tree implementation. */
		/*!
		 *      This is a tree which does NOT allow duplicate keys.
		 */
		template <class Key, class Data, bool OwnsKeys = true>
		class SplayTree
		{
			private:

				/*! \brief Private copy constructor. */
				/*!
				 * If your code needs to invoke the copy constructor, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 */
				SplayTree(const SplayTree<Key, Data, OwnsKeys> &) = delete;

				/*! \brief Private assignment operator. */
				/*!
				 * If your code needs to invoke the assignment operator, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 */
				SplayTree<Key, Data, OwnsKeys> &operator =(const SplayTree<Key, Data, OwnsKeys> &) = delete;

				/* Mutable because splaying is an operation which changes */
				/* the structure of the tree. */
				mutable SplayNode<Key, Data, OwnsKeys> *root;

				void printTree(SplayNode<Key, Data, OwnsKeys> *t) const;

				/* Tree manipulations */
				void rotateWithLeftChild(SplayNode<Key, Data, OwnsKeys> * &k2) const;
				void rotateWithRightChild(SplayNode<Key, Data, OwnsKeys> * &k1) const;
				void splay(Key const &key, SplayNode<Key, Data, OwnsKeys> * &t) const;

				SplayNode<Key, Data, OwnsKeys> *findNode(Key const &key) const;

				size_t m_size;

				void RecursiveConvertIndexToDArray(DArray <Key> *_darray, SplayNode<Key, Data, OwnsKeys> *_btree) const;

				template <class TypedData>
				void RecursiveConvertToDArray(DArray <TypedData> *_darray, SplayNode<Key, Data, OwnsKeys> *_btree) const;

				bool erase(Key const &key, Data const &rec, SplayNode<Key, Data, OwnsKeys> *curnode);

				bool killNode(SplayNode<Key, Data, OwnsKeys> * z);

			public:

				/*! \brief The default constructor. */
				SplayTree();

				/*! \brief The destructor. */
				~SplayTree();

				/*! \brief Empties the entire tree. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed
				 *    separately. The preferred way to do this is to serialize the data into a DArray
				 *    with ConvertToDArray() and then iterate through it to delete the data in whatever
				 *    way is proper.
				 */
				void empty();

				/*! \brief Inserts data into the tree. */
				/*!
				 * \param _key The key of the data.
				 * \param _rec The data to insert.
				 * \return True on success, false on failure.
				 */
				bool insert(Key const &_key, Data const &_rec);

				/*! \brief Tests whether a key is in the tree or not. */
				/*!
				 * \param _key The key of the node to find.
				 * \return True if the key is in the tree, false if not.
				 */
				bool exists(Key const &_key) const;

				/*! \brief Change the data at the given node. */
				/*!
				 * \param _key The key of the node to be modified.
				 * \param _rec The data to insert.
				 * \return True on success, false on failure.
				 */
				bool replace(Key const &_key, Data const &_rec);

				/*! \brief Finds a node in the tree and returns the data at that node. */
				/*!
				 * \param _key The key of the node to find.
				 * \param _default The value to return if the item couldn't be found.
				 * \return If found, returns the data at the node, otherwise _default is returned.
				 */
				template <class TypedData = Data>
				TypedData find(Key const &_key, TypedData const &_default = nullptr) const;

				/*! \brief Deletes a node from the tree, specified by the node's key. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
				 * \param _key The key of the node to delete.
				 * \return True on success, false on failure
				 */
				bool erase(Key const &_key);

				/*! \brief Indicates the size of the tree. */
				/*!
				 * \return Size of the tree.
				 */
				inline size_t size() const
				{
					return m_size;
				}

				/*! \brief Converts the tree data into a linearized DArray. */
				/*!
				 * \return A DArray containing the data of the tree.
				 * \warning Delete the returned DArray when done with it.
				 */
				template <class TypedData = Data>
				DArray <TypedData> *ConvertToDArray() const;

				/*! \brief Converts the tree keys into a linearized DArray. */
				/*!
				 * \return A DArray containing the keys in the tree.
				 * \warning Delete the returned DArray when done with it.
				 */
				DArray <Key> *ConvertIndexToDArray() const;

				/*! \brief Returns the overhead caused by the data structure. */
				/*!
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage() const;

#if !defined (DISABLE_DEPRECATED_CODE)
				/*
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				_CC_DEPRECATE_FUNCTION(insert)  inline void     PutData(Key const &_key, Data const & _rec)
				{
					insert(_key, _rec);
				}
				_CC_DEPRECATE_FUNCTION(find)    inline Data GetData(Key const &_key, Data const &_default = NULL)
				{
					return find(_key, _default);
				}
				_CC_DEPRECATE_FUNCTION(erase)   inline void     RemoveData(Key const &_key)
				{
					erase(_key);
				}
				_CC_DEPRECATE_FUNCTION(erase)   inline void     RemoveData(Key const &_key, Data const & _rec)
				{
					erase(_key, _rec);
				}
				_CC_DEPRECATE_FUNCTION(size)    inline int      Size() const
				{
					return (int)size();
				}
				_CC_DEPRECATE_FUNCTION(empty)   inline void     Empty()
				{
					empty();
				}
				/*! @endcond */
#endif
		};
	}
}

#include <crisscross/splaytree.cpp>

#endif
