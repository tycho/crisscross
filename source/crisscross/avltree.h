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

#ifndef __included_cc_avltree_h
#define __included_cc_avltree_h

#include <crisscross/cc_attr.h>
#include <crisscross/internal_mem.h>
#include <crisscross/darray.h>
#include <crisscross/node.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A very fast AVL tree implementation. */
		/*! AVL trees are quite useful for tasks that require very fast and
		 * well-balanced trees. Due to the ruleset implemented internally,
		 * AVL trees enforce a maximum height of 1.44*log(n).
		 */
		template <class Key, class Data, bool OwnsKeys = true>
		class AVLTree
		{
			private:
				/*! \brief Private copy constructor. */
				/*!
				 * If your code needs to invoke the copy constructor, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 */
				AVLTree(const AVLTree<Key, Data, OwnsKeys> &);

				/*! \brief Private assignment operator. */
				/*!
				 * If your code needs to invoke the assignment operator, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 */
				AVLTree<Key, Data, OwnsKeys> &operator =(const AVLTree<Key, Data, OwnsKeys> &);

			protected:
				/*! \brief The root node. */
				AVLNode<Key, Data, OwnsKeys> *m_root;

				/*! \brief The current tree size. */
				size_t m_size;

				/*! \brief Result of tree operation */
				enum
				{
					/*! \brief None of the subtrees has grown in height, entire tree is still balanced */
					OK,

					/*! \brief One of the branches has grown/shrunk in height, tree might need rebalancing */
					BALANCE,

					/*! \brief Error */
					INVALID
				};

				/*! \brief Rotate tree left */
				/*!
				 * Rotate tree left around the given node
				 * \param _node Pointer to current node pointer to rotate
				 */
				inline void rotateLeft(AVLNode<Key, Data, OwnsKeys> **_node);

				/*! \brief Rotate tree right */
				/*!
				 * Rotate tree right around the given node
				 * \param _node Pointer to current node pointer to rotate
				 */
				inline void rotateRight(AVLNode<Key, Data, OwnsKeys> **_node);

				/*! \brief Rebalance tree */
				/*!
				 * Rebalance tree after left side has grown
				 * \param _node Pointer to current node pointer to balance
				 * \return OK if tree is balanced (entire tree is valid), BALANCE if local tree is balanced but has grown in height (entire tree not guaranteed to be valid)
				 */
				inline int balanceLeftGrown(AVLNode<Key, Data, OwnsKeys> **_node);

				/*! \brief Rebalance tree */
				/*!
				 * Rebalance tree after right side has grown
				 * \param _node Pointer to current node pointer to balance
				 * \return OK if tree is balanced (entire tree is valid), BALANCE if local tree is balanced but has grown in height (entire tree not guaranteed to be valid)
				 */
				inline int balanceRightGrown(AVLNode<Key, Data, OwnsKeys> **_node);

				/*! \brief Rebalance tree */
				/*!
				 * Rebalance tree after left side has shrunk
				 * \param _node Pointer to current node pointer to balance
				 * \return OK if tree is balanced (entire tree is valid), BALANCE if local tree is balanced but has shrunk in height (entire tree not guaranteed to be valid)
				 */
				inline int balanceLeftShrunk(AVLNode<Key, Data, OwnsKeys> **_node);

				/*! \brief Rebalance tree */
				/*!
				 * Rebalance tree after right side has shrunk
				 * \param _node Pointer to current node pointer to balance
				 * \return OK if tree is balanced (entire tree is valid), BALANCE if local tree is balanced but has shrunk in height (entire tree not guaranteed to be valid)
				 */
				inline int balanceRightShrunk(AVLNode<Key, Data, OwnsKeys> **_node);

				/*! \brief Replace node */
				/*!
				 * Replace a node with the highest-ranking item in subtree
				 * \param _target Pointer to node to be replaced
				 * \param _subtree Pointer to subtree pointer to search
				 * \param _result Pointer to result variable to tell caller if further checks are needed
				 * \return true if node found, false if not
				 */
				inline bool replaceWithHighest(AVLNode<Key, Data, OwnsKeys> *_target, AVLNode<Key, Data, OwnsKeys> **_subtree, int *_result);

				/*! \brief Replace node */
				/*!
				 * Replace a node with the lowest-ranking item in subtree
				 * \param _target Pointer to node to be replaced
				 * \param _subtree Pointer to subtree pointer to search
				 * \param _result Pointer to result variable to tell caller if further checks are needed
				 * \return true if node found, false if not
				 */
				inline bool replaceWithLowest(AVLNode<Key, Data, OwnsKeys> *_target, AVLNode<Key, Data, OwnsKeys> **_subtree, int *_result);

				/*! \brief Add object */
				/*!
				 * Insert object in tree and rebalance
				 * \param _parent Pointer to parent node pointer
				 * \param _node Pointer to current node pointer
				 * \param _key Key to insert
				 * \param _data Data to insert
				 * \return int of addition (OK if subtree is balanced, BALANCE if tree is heavy on either side)
				 */
				int insert(AVLNode<Key, Data, OwnsKeys> **_parent, AVLNode<Key, Data, OwnsKeys> **_node, Key const &_key, Data const &_data);

				/*! \brief Remove object */
				/*!
				 * Remove object from tree and rebalance
				 * \param _node Pointer to current node pointer
				 * \param _key Identifier of node to remove
				 * \return int of removal (OK if subtree is balanced, BALANCE if tree is heavy on either side)
				 */
				int erase(AVLNode<Key, Data, OwnsKeys> **_node, Key const &_key);

				/*! \brief Find a node in the tree */
				/*!
				 * Get a pointer to a node with the specified key value
				 * \param _key Identifier of node to remove
				 * \return Address of the node. If not found, returns nullptr.
				 */
				AVLNode<Key, Data, OwnsKeys> *findNode(Key const &_key) const;

				/*! \brief Recursively convert the tree's keys into a DArray */
				/*!
				 * \param _darray Array to insert keys into
				 * \param _btree The node being traversed
				 */
				void RecursiveConvertIndexToDArray(DArray <Key> *_darray, AVLNode<Key, Data, OwnsKeys> *_btree) const;

				/*! \brief Recursively convert the tree's data into a DArray */
				/*!
				 * \param _darray Array to insert data into
				 * \param _btree The node being traversed
				 */
				template <class TypedData>
				void RecursiveConvertToDArray(DArray <TypedData> *_darray, AVLNode<Key, Data, OwnsKeys> *_btree) const;

				/*! \brief Verifies that a node is valid. */
				/*!
				 * \param _node A node pointer.
				 * \return True if the node is a valid node, false otherwise.
				 */
				inline bool valid(const AVLNode<Key, Data, OwnsKeys> *_node) const
				{
					return (_node != nullptr);
				}

			public:

				/*! \brief The default constructor. */
				AVLTree();

				/*! \brief The destructor. */
				virtual ~AVLTree();

				/*! \brief Inserts data into the tree. */
				/*!
				 * \param _key The key of the data.
				 * \param _data The data to insert.
				 * \return True on success, false on failure.
				 */
				bool insert(Key const &_key, Data const &_data);

				/*! \brief Deletes a node from the tree, specified by the node's key. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
				 * \param _key The key of the node to delete.
				 * \return True on success, false on failure
				 */
				bool erase(Key const &_key);

				/*! \brief Deletes a node from the tree, specified by the node's key and data. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed separately.
				 * \param _key The key of the node to delete.
				 * \param _data The data of the node to delete.
				 * \return True on success, false on failure.
				 */
				bool erase(Key const &_key, Data const &_data);

				/*! \brief Finds a node in the tree and returns the data at that node. */
				/*!
				 * \param _key The key of the node to find.
				 * \param _default The value to return if the item couldn't be found.
				 * \return If found, returns the data at the node, otherwise _default is returned.
				 */
				template <class TypedData = Data>
				TypedData find(Key const &_key, TypedData const &_default = nullptr) const;

				/*! \brief Tests whether a key is in the tree or not. */
				/*!
				 * \param _key The key of the node to find.
				 * \return True if the key is in the tree, false if not.
				 */
				bool exists(Key const &_key) const;

				/*! \brief Empties the entire tree. */
				/*!
				 * \warning This won't free the memory occupied by the data, so the data must be freed
				 *    separately. The preferred way to do this is to serialize the data into a DArray
				 *    with ConvertToDArray() and then iterate through it to delete the data in whatever
				 *    way is proper.
				 */
				inline void empty()
				{
					delete m_root; m_root = nullptr; m_size = 0;
				}

				/*! \brief Indicates the size of the tree. */
				/*!
				 * \return Size of the tree.
				 */
				inline size_t size() const
				{
					return m_size;
				}

				/*! \brief Change the data at the given node. */
				/*!
				 * \param _key The key of the node to be modified.
				 * \param _data The data to insert.
				 * \return True on success, false on failure.
				 */
				bool replace(Key const &_key, Data const &_data);

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
				_CC_DEPRECATE_FUNCTION(find)    inline Data	    GetData(Key const &_key, Data const &_default = nullptr)
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

#include <crisscross/avltree.cpp>

#endif
