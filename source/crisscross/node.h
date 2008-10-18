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

#ifndef __included_cc_node_h
#define __included_cc_node_h

#include <crisscross/internal_mem.h>

namespace CrissCross
{
	namespace Data
	{
		/*
		 *      There's no real advantage to making these classes inherit
		 *      a common BinaryNode class. In fact, it will impact the
		 *      program negatively to inherit a common class because we
		 *      are forced to either cast every reference to left/right/parent
		 *      or override left/right/parent in the derived classes (and
		 *      doing so would waste memory: 12 bytes per node on 32-bit
		 *      machines).
		 *
		 *      So we no longer inherit a common node class.
		 */

		/*! \brief A binary tree node. */
		template <class Key, class Data>
		class SplayNode
		{
			public:
				/*! \brief The key for this node. */
				Key        id;

				/*! \brief The data held at this node. */
				Data       data;

				/*! \brief The left branch of the tree from this node. */
				SplayNode *left;

				/*! \brief The right branch of the tree from this node. */
				SplayNode *right;

				/*! \brief The parent node. */
				SplayNode *parent;

				/*! \brief The constructor. */
				SplayNode() : left(NULL), right(NULL), parent(NULL)
				{
				}

				/*! \brief The destructor. */
				~SplayNode()
				{
					Dealloc(id);
					delete left; left = NULL;
					delete right; right = NULL;
				}

				/*! \brief Returns the overhead caused by the node. */
				/*!
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage() const
				{
					size_t ret = sizeof(*this);
					if (left) ret += left->mem_usage();

					if (right) ret += right->mem_usage();

					return ret;
				}
		};

		/*! \brief The current balance status of a node */
		typedef enum
		{
			/*! \brief The left side of the tree is heaviest. */
			LEFTHEAVY,

			/*! \brief The tree is well balanced. */
			BALANCED,

			/*! \brief The right side of the tree is heaviest. */
			RIGHTHEAVY
		} AVLBalance;

		/*! \brief A binary tree node used for AVLTree. */
		template <class Key, class Data>
		class AVLNode
		{
			public:

				/*! \brief The left branch of the tree from this node. */
				AVLNode      *left;

				/*! \brief The right branch of the tree from this node. */
				AVLNode      *right;

				/*! \brief The parent node. */
				AVLNode      *parent;

				/*! \brief The key for this node. */
				Key           id;

				/*! \brief The data held at this node. */
				Data          data;

				/*! \brief The state of this part of the tree's balance. */
				unsigned char balance : 2;

				/*! \brief The default constructor. */
				AVLNode() : left(NULL), right(NULL), parent(NULL), balance(BALANCED)
				{
				}

				/*! \brief The destructor. */
				~AVLNode()
				{
					Dealloc(id);
					delete left; left = NULL;
					delete right; right = NULL;
				}

				/*! \brief Returns the overhead caused by the node. */
				/*!
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage() const
				{
					size_t ret = sizeof(*this);
					if (left) ret += left->mem_usage();

					if (right) ret += right->mem_usage();

					return ret;
				}
		};

		template <class Key, class Data>
		class RedBlackTree;

		/*! \brief A binary tree node used for RedBlackTree. */
		template <class Key, class Data>
		class RedBlackNode
		{
			public:

				/*! \brief The left branch of the tree from this node. */
				RedBlackNode *left;

				/*! \brief The right branch of the tree from this node. */
				RedBlackNode *right;

				/*! \brief The parent node. */
				RedBlackNode *parent;

				/*! \brief The key for this node. */
				Key           id;

				/*! \brief The data held at this node. */
				Data          data;

				/*! \brief The color of the node (either red or black). */
				unsigned char color : 1;

				/*! \brief The default constructor. */
				RedBlackNode() : left(NULL), right(NULL), parent(NULL)
				{
				}

				/*! \brief The destructor. */
				~RedBlackNode()
				{
					Dealloc(id);
					delete left; left = NULL;
					delete right; right = NULL;
				}


				/*! \brief Returns the overhead caused by the node. */
				/*!
				 * \param _parentTree The tree which contains this node instance.
				 * \return Memory usage in bytes.
				 */
				size_t mem_usage(CrissCross::Data::RedBlackTree<Key, Data> const *_parentTree) const
				{
					size_t ret = sizeof(*this);
					if (left && left != _parentTree->nullNode) ret += left->mem_usage(_parentTree);

					if (right && right != _parentTree->nullNode) ret += right->mem_usage(_parentTree);

					return ret;
				}
		};
	}
}

#endif
