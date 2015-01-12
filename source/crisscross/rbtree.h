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

#ifndef __included_cc_rbtree_h
#define __included_cc_rbtree_h

#include <crisscross/cc_attr.h>
#include <crisscross/deprecate.h>
#include <crisscross/internal_mem.h>
#include <crisscross/darray.h>
#include <crisscross/node.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A very fast red-black tree implementation. */
		/*!
		 * Red-black trees are complex, but have good worst-case running time for their
		 * operations and are efficient in practice: they can search, insert, and delete
		 * in O(log n) time, where n is total number of elements in the tree.
		 */
		template <class Key, class Data>
		class RedBlackTree
		{
			private:
				/*! \brief Private copy constructor. */
				/*!
				 * If your code needs to invoke the copy constructor, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 */
				RedBlackTree(const RedBlackTree<Key, Data> &);

				/*! \brief Private assignment operator. */
				/*!
				 * If your code needs to invoke the assignment operator, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 */
				RedBlackTree<Key, Data> &operator =(const RedBlackTree<Key, Data> &);

			protected:

				/*! \brief The root node at the top of the tree. */
				RedBlackNode<Key, Data> *rootNode;

				/*! \brief The "null" node. Added so we don't need special cases to check for null pointers. */
				RedBlackNode<Key, Data> *nullNode;

				/*! \brief The cached size() return value. Changes on each tree modification (insertions and deletions). */
				size_t m_cachedSize;

				void RecursiveConvertIndexToDArray(DArray <Key> *_darray, RedBlackNode<Key, Data> *_btree) const;
				void RecursiveConvertToDArray(DArray <Data> *_darray, RedBlackNode<Key, Data> *_btree) const;

				void rotateLeft(RedBlackNode<Key, Data> * _x);
				void rotateRight(RedBlackNode<Key, Data> * _x);
				void insertFixup(RedBlackNode<Key, Data> * _x);
				void deleteFixup(RedBlackNode<Key, Data> * _x);

				void killAll();
				void killAll(RedBlackNode<Key, Data> *rec);

				bool killNode(RedBlackNode<Key, Data> * z);

				RedBlackNode<Key, Data> *findNode(Key const &key) const;

				/*! \brief Verifies that a node is valid. */
				/*!
				 * \param _node A node pointer.
				 * \return True if the node is a valid node, false otherwise.
				 */
				inline bool valid(const RedBlackNode<Key, Data> *_node) const
				{
					return (_node != NULL && _node != nullNode);
				}

			public:
				/*! @cond */
				typedef enum { BLACK, RED } nodeColor;
				/*! @endcond */

				/*! \brief The constructor. */
				RedBlackTree();

				/*! \brief The destructor. */
				~RedBlackTree();

				/*! \brief Inserts data into the tree. */
				/*!
				 * \param _key The key of the data.
				 * \param _rec The data to insert.
				 * \return True on success, false on failure.
				 */
				bool insert(Key const &_key, Data const &_rec);

				/*! \brief Change the data at the given node. */
				/*!
				 * \param _key The key of the node to be modified.
				 * \param _rec The data to insert.
				 * \return True on success, false on failure.
				 */
				bool replace(Key const &_key, Data const &_rec);

				/*! \brief Deletes a node from the tree, specified by the node's key. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
				 * \param _key The key of the node to delete.
				 * \return True on success, false on failure
				 */
				bool erase(Key const &_key);

				/*! \brief Finds a node in the tree and returns the data at that node. */
				/*!
				 * \param _key The key of the node to find.
				 * \param _default The value to return if the item couldn't be found.
				 * \return If found, returns the data at the node, otherwise _default is returned.
				 */
				Data find(Key const &_key, Data const &_default = NULL) const;

				/*! \brief Empties the entire tree. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed
				 *     separately. The preferred way to do this is to serialize the data into a DArray
				 *     with ConvertToDArray() and then iterate through it to delete the data in whatever
				 *     way is proper.
				 */
				inline void empty()
				{
					killAll();
				}

				/*! \brief Indicates the size of the tree. */
				/*!
				 * \return Size of the tree.
				 */
				__forceinline size_t size() const
				{
					return m_cachedSize;
				}

				/*! \brief Tests whether a key is in the tree or not. */
				/*!
				 * \param _key The key of the node to find.
				 * \return True if the key is in the tree, false if not.
				 */
				bool exists(Key const &_key) const;

				/*! \brief Converts the tree data into a linearized DArray. */
				/*!
				 * \return A DArray containing the data of the tree.
				 * \warning Delete the returned DArray when done with it.
				 */
				DArray <Data> *ConvertToDArray() const;

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
				 * Deprecated Compatibility Functions
				 * Provided for compatibility with Tosser I
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

#if !defined(_MSC_VER) || _MSC_VER >= 1400
				template <class K, class D>
#endif
				friend class RedBlackNode;
		};
	}
}

#include <crisscross/rbtree.cpp>

#endif
