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

#ifndef __included_cc_vtree_h
#define __included_cc_vtree_h

#include <crisscross/platform_detect.h>

#ifdef TARGET_SUPPORTS_TR1

#include <crisscross/cc_attr.h>
#include <crisscross/cow_ptr.h>
#include <crisscross/deprecate.h>
#include <crisscross/node.h>
#include <crisscross/internal_mem.h>

namespace CrissCross
{
	namespace Data
	{

		/*! \brief A very simple binary search tree. */
		/*! This binary search tree is designed to be a template for
		 * future tree designs and is not intended for general use. It
		 * has no self-balancing features, but is a fully functional
		 * binary search tree.
		 */
		template <class Key, class Data>
		class VTree
		{
			private:
				/*! \brief Private copy constructor. */
				/*!
				 * If your code needs to invoke the copy constructor, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 * \deprecated This is a reference implementation. Do not use it for real world
				 * applications.
				 */
				VTree(const VTree<Key, Data> &);

				/*! \brief Private assignment operator. */
				/*!
				 * If your code needs to invoke the assignment operator, you've probably written
				 * the code wrong. A tree copy is generally unnecessary, and in cases that it
				 * is, it can be achieved by other means.
				 */
				VTree<Key, Data> &operator =(const VTree<Key, Data> &);

			protected:
				/*! \brief The root node. */
				cow_ptr<VNode<Key, Data>> m_root;

				/*! \brief The current tree size. */
				size_t m_size;

				/*! \brief Find a node in the tree */
				/*!
				 * Get a pointer to a node with the specified key value
				 * \param _key Identifier of node to remove
				 * \return Address of the node. If not found, returns NULL.
				 */
				cow_ptr<VNode<Key, Data>> findNode(Key const &_key) const;

				/*! \brief Recursively convert the tree's keys into a DArray */
				/*!
				 * \param _darray Array to insert keys into
				 * \param _btree The node being traversed
				 */
				void RecursiveConvertIndexToDArray(DArray <Key> *_darray, VNode<Key, Data> *_btree) const;

				/*! \brief Recursively convert the tree's data into a DArray */
				/*!
				 * \param _darray Array to insert data into
				 * \param _btree The node being traversed
				 */
				void RecursiveConvertToDArray(DArray <Data> *_darray, VNode<Key, Data> *_btree) const;

				/*! \brief Verifies that a node is valid. */
				/*!
				 * \param _node A node pointer.
				 * \return True if the node is a valid node, false otherwise.
				 */
				inline bool valid(cow_ptr<VNode<Key, Data>> const &_node) const
				{
					return (_node.get() != NULL);
				}

				/*! \brief Checks for the existence of a left-hand child. */
				/*!
				 * \param _node A node pointer.
				 * \return True if the node has a left-hand child, false otherwise.
				 */
				inline cow_ptr<VNode<Key, Data>> const &parent(cow_ptr<VNode<Key, Data>> const &_node) const
				{
					return _node.get()->parent;
				}

				/*! \brief Checks for the existence of a right-hand child. */
				/*!
				 * \param _node A node pointer.
				 * \return True if the node has a right-hand child, false otherwise.
				 */
				inline bool has_right_child(cow_ptr<VNode<Key, Data>> const &_node) const
				{
					return (_node.get()->right.get() != NULL);
				}

				/*! \brief Checks for the existence of a left-hand child. */
				/*!
				 * \param _node A node pointer.
				 * \return True if the node has a left-hand child, false otherwise.
				 */
				inline bool has_left_child(cow_ptr<VNode<Key, Data>> const &_node) const
				{
					return (_node.get()->left.get() != NULL);
				}

			public:

				/*! \brief The default constructor. */
				VTree();

				/*! \brief The destructor. */
				virtual ~VTree();

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
				 * \return True on success, false on failure.
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
				Data find(Key const &_key, Data const &_default = NULL) const;

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
					delete m_root; m_root = NULL; m_size = 0;
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
				 *      Deprecated Compatibility Functions
				 *      Provided for compatibility with Tosser I
				 */
				/*! @cond */
				_CC_DEPRECATE_FUNCTION(insert)  inline void     PutData(Key const &_key, Data const & _rec)
				{
					insert(_key, _rec);
				}
				_CC_DEPRECATE_FUNCTION(find)    inline Data	    GetData(Key const &_key, Data const &_default = NULL)
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

#include <crisscross/vtree.cpp>

#endif

#endif
