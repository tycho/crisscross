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

#ifndef __included_cc_node_h
#define __included_cc_node_h

#include <crisscross/internal_mem.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A binary tree node used for SplayTree. */
		template <class Key, class Data, bool OwnsKeys>
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
				SplayNode() : left(nullptr), right(nullptr), parent(nullptr)
				{
				}

				/*! \brief The destructor. */
				~SplayNode()
				{
					if (OwnsKeys)
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
		template <class Key, class Data, bool OwnsKeys>
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
				AVLNode() : left(nullptr), right(nullptr), parent(nullptr), balance(BALANCED)
				{
				}

				/*! \brief The destructor. */
				~AVLNode()
				{
					if (OwnsKeys)
						Dealloc(id);
					delete left; left = nullptr;
					delete right; right = nullptr;
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
		template <class Key, class Data, bool OwnsKeys>
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
				SNode() : left(nullptr), right(nullptr), parent(nullptr)
				{
				}

				/*! \brief The destructor. */
				~SNode()
				{
					if (OwnsKeys)
						Dealloc(id);
					delete left; left = nullptr;
					delete right; right = nullptr;
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

		/*! \brief A binary tree node used for RedBlackTree. */
		template <class Key, class Data, bool OwnsKeys>
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
				RedBlackNode() : left(nullptr), right(nullptr), parent(nullptr)
				{
				}

				/*! \brief The destructor. */
				~RedBlackNode()
				{
					if (OwnsKeys)
						Dealloc(id);
					delete left; left = nullptr;
					delete right; right = nullptr;
				}
		};
	}
}

#endif
