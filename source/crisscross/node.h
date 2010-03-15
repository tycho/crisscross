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

#ifndef __included_cc_node_h
#define __included_cc_node_h

#include <crisscross/platform_detect.h>
#include <crisscross/cow_ptr.h>
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
				Key id;

				/*! \brief The data held at this node. */
				Data data;

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
				AVLNode *left;

				/*! \brief The right branch of the tree from this node. */
				AVLNode *right;

				/*! \brief The parent node. */
				AVLNode *parent;

				/*! \brief The key for this node. */
				Key id;

				/*! \brief The data held at this node. */
				Data data;

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

		/*! \brief A binary tree node used for STree. */
		template <class Key, class Data>
		class SNode
		{
			public:

				/*! \brief The left branch of the tree from this node. */
				SNode *left;

				/*! \brief The right branch of the tree from this node. */
				SNode *right;

				/*! \brief The parent node. */
				SNode *parent;

				/*! \brief The key for this node. */
				Key id;

				/*! \brief The data held at this node. */
				Data data;

				/*! \brief The default constructor. */
				SNode() : left(NULL), right(NULL), parent(NULL)
				{
				}

				/*! \brief The destructor. */
				~SNode()
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

#ifdef TARGET_SUPPORTS_TR1
		/*! \brief A binary tree node used for VTree. */
		template <class Key, class Data>
		class VNode
		{
			public:

				/*! \brief The left branch of the tree from this node. */
				cow_ptr<VNode<Key, Data>> left;

				/*! \brief The right branch of the tree from this node. */
				cow_ptr<VNode<Key, Data>> right;

				/*! \brief The parent node. */
				cow_ptr<VNode<Key, Data>> parent;

				/*! \brief The key for this node. */
				Key id;

				/*! \brief The data held at this node. */
				Data data;

				/*! \brief The default constructor. */
				VNode()
				{
				}

				/*! \brief The default constructor. */
				VNode(Key const &_key, Data const &_data)
				{
					id = Duplicate(_key);
					data = _data;
				}

				/*! \brief The copy constructor. */
				VNode(VNode const &orig)
				{
					id = Duplicate<Key>(orig.id);
					data = orig.data;
					left = orig.left;
					right = orig.right;
					parent = orig.parent;
				}

				/*! \brief The destructor. */
				~VNode()
				{
					Dealloc(id);
					left.reset();
					right.reset();
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
#endif

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
				Key id;

				/*! \brief The data held at this node. */
				Data data;

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
		};
	}
}

#endif
