/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2015 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_rbtree_h
#error "This file shouldn't be compiled directly."
#endif

#include <stdlib.h>

#include <crisscross/compare.h>
#include <crisscross/debug.h>
#include <crisscross/rbtree.h>

namespace CrissCross
{
	namespace Data
	{
		template <class Key, class Data>
		RedBlackTree<Key, Data>::RedBlackTree()
		{
			nullNode = new RedBlackNode<Key, Data>();
			nullNode->left = nullNode->right = nullNode->parent = nullNode;
			nullNode->color = BLACK;
			nullNode->id = NullKey<Key>();
			rootNode = nullNode;
			m_cachedSize = 0;
		}

		template <class Key, class Data>
		RedBlackTree<Key, Data>::~RedBlackTree()
		{
			killAll();
			nullNode->right = nullNode->left = NULL;
			delete nullNode;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::rotateLeft(RedBlackNode<Key, Data> * x)
		{
			RedBlackNode<Key, Data> *y = x->right;

			/* establish x->right link */
			x->right = y->left;
			if (valid(y->left))
				y->left->parent = x;

			/* establish y->parent link */
			if (valid(y))
				y->parent = x->parent;

			if (valid(x->parent)) {
				if (x == x->parent->left)
					x->parent->left = y;
				else
					x->parent->right = y;
			} else {
				rootNode = y;
			}

			/* link x and y */
			y->left = x;
			if (valid(x))
				x->parent = y;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::rotateRight(RedBlackNode<Key, Data> * x)
		{
			RedBlackNode<Key, Data> *y = x->left;

			/* establish x->left link */
			x->left = y->right;
			if (valid(y->right))
				y->right->parent = x;

			/* establish y->parent link */
			if (valid(y))
				y->parent = x->parent;

			if (valid(x->parent)) {
				if (x == x->parent->right)
					x->parent->right = y;
				else
					x->parent->left = y;
			} else {
				rootNode = y;
			}

			/* link x and y */
			y->right = x;
			if (valid(x))
				x->parent = y;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::insertFixup(RedBlackNode<Key, Data> * x)
		{
			/* check Red-Black properties */
			while (x != rootNode && x->parent->color == RED) {
				/* we have a violation */
				if (x->parent == x->parent->parent->left) {
					RedBlackNode<Key, Data> *y = x->parent->parent->right;

					if (valid(y) && y->color == RED) {
						/* uncle is RED */
						x->parent->color = BLACK;
						y->color = BLACK;
						x->parent->parent->color = RED;
						x = x->parent->parent;
					} else {
						/* uncle is BLACK */
						if (x == x->parent->right) {
							/* make x a left child */
							x = x->parent;
							rotateLeft(x);
						}

						/* recolor and rotate */
						x->parent->color = BLACK;
						x->parent->parent->color = RED;
						rotateRight(x->parent->parent);
					}
				} else {
					/* mirror image of above code */
					RedBlackNode<Key, Data> *y = x->parent->parent->left;

					if (valid(y) && y->color == RED) {
						/* uncle is RED */
						x->parent->color = BLACK;
						y->color = BLACK;
						x->parent->parent->color = RED;
						x = x->parent->parent;
					} else {
						/* uncle is BLACK */
						if (x == x->parent->left) {
							x = x->parent;
							rotateRight(x);
						}

						x->parent->color = BLACK;
						x->parent->parent->color = RED;
						rotateLeft(x->parent->parent);
					}
				}
			}
			rootNode->color = BLACK;
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::replace(Key const &key, Data const & rec)
		{
			RedBlackNode<Key, Data> *current;
			current = findNode(key);
			if (!valid(current)) return false;

			current->data = rec;
			return true;
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::insert(Key const &key, Data const & rec)
		{
			RedBlackNode<Key, Data> *current, *parent = nullNode, *x = nullNode;

			/* find future parent */
			current = rootNode;
			while (valid(current)) {
				parent = current;
				int ret = Compare(key, current->id);
				if (ret < 0)
					current = current->left;
				else if (ret > 0)
					current = current->right;
				else
					return false;
			}

			/* setup new node */
			if ((x = new RedBlackNode<Key, Data>()) == 0) {
				return false;
			}

			x->parent = parent;
			x->left = nullNode;
			x->right = nullNode;
			x->color = RED;
			x->id = Duplicate(key);
			x->data = rec;

			/* insert node in tree */
			if (valid(parent)) {
				if (Compare(key, parent->id) <= 0)
					parent->left = x;
				else
					parent->right = x;
			} else {
				rootNode = x;
			}

			m_cachedSize++;

			insertFixup(x);

			return true;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::deleteFixup(RedBlackNode<Key, Data> * x)
		{
			if (!valid(x)) return;

			while (x != rootNode && x->color == BLACK) {
				if (x == x->parent->left) {
					RedBlackNode<Key, Data> *w = x->parent->right;

					if (w->color == RED) {
						w->color = BLACK;
						x->parent->color = RED;
						rotateLeft(x->parent);
						w = x->parent->right;
					}

					if (w->left->color == BLACK && w->right->color == BLACK) {
						w->color = RED;
						x = x->parent;
					} else {
						if (w->right->color == BLACK) {
							w->left->color = BLACK;
							w->color = RED;
							rotateRight(w);
							w = x->parent->right;
						}

						w->color = x->parent->color;
						x->parent->color = BLACK;
						w->right->color = BLACK;
						rotateLeft(x->parent);
						x = rootNode;
					}
				} else {
					RedBlackNode<Key, Data> *w = x->parent->left;

					if (w->color == RED) {
						w->color = BLACK;
						x->parent->color = RED;
						rotateRight(x->parent);
						w = x->parent->left;
					}

					if (w->right->color == BLACK && w->left->color == BLACK) {
						w->color = RED;
						x = x->parent;
					} else {
						if (w->left->color == BLACK) {
							w->right->color = BLACK;
							w->color = RED;
							rotateLeft(w);
							w = x->parent->left;
						}

						w->color = x->parent->color;
						x->parent->color = BLACK;
						w->left->color = BLACK;
						rotateRight(x->parent);
						x = rootNode;
					}
				}
			}
			x->color = BLACK;
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::erase(Key const &key)
		{
			RedBlackNode<Key, Data> *z;

			/*  delete node z from tree */

			/* find node in tree */
			z = rootNode;

			while (valid(z)) {
				if (Compare(key, z->id) == 0)
					break;
				else{
					z = (Compare(key, z->id) <= 0) ? z->left : z->right;
				}
			}

			if (z == nullNode) {
				return false;
			}

			return killNode(z);
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::killNode(RedBlackNode<Key, Data> * z)
		{
			RedBlackNode<Key, Data> *x, *y;

			if (!valid(z->left) || !valid(z->right)) {
				/* y has a null node as a child */
				y = z;
			} else {
				/* find tree successor with a null node as a child */
				y = z->right;

				while (valid(y->left))
					y = y->left;
			}

			/* x is y's only child */
			if (valid(y->left))
				x = y->left;
			else
				x = y->right;

			/* remove y from the parent chain */
			if (valid(x)) x->parent = y->parent;

			if (valid(y->parent)) {
				if (y == y->parent->left)
					y->parent->left = x;
				else
					y->parent->right = x;
			} else
				rootNode = x;

			if (y != z) {
				Dealloc(z->id);
				z->id = Duplicate(y->id);
				z->data = y->data;
			} else {
				Dealloc(y->id);
			}

			if (y->color == BLACK)
				deleteFixup(x);

			m_cachedSize--;

			y->left = NULL;
			y->right = NULL;
			delete y;

			return true;
		}

		template <class Key, class Data>
		Data RedBlackTree<Key, Data>::find(Key const &_key, Data const &_default) const
		{
			RedBlackNode<Key, Data> *p_current = rootNode;

			while (valid(p_current)) {
				int cmp = Compare(_key, p_current->id);
				if (cmp < 0)
					p_current = p_current->left;
				else if (cmp > 0)
					p_current = p_current->right;
				else {
					return p_current->data;
				}
			}

			return _default;
		}

		template <class Key, class Data>
		RedBlackNode<Key, Data> * RedBlackTree<Key, Data>::findNode(Key const &_key) const
		{
			RedBlackNode<Key, Data> *p_current = rootNode;

			while (valid(p_current)) {
				int cmp = Compare(_key, p_current->id);
				if (cmp < 0)
					p_current = p_current->left;
				else if (cmp > 0)
					p_current = p_current->right;
				else {
					return p_current;
				}
			}

			return NULL;
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::exists(Key const &_key) const
		{
			RedBlackNode<Key, Data> *p_current = findNode(_key);
			if (!valid(p_current)) return false;
			else return true;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::killAll(RedBlackNode<Key, Data> *rec)
		{
			if (!valid(rec)) {
				return;
			}

			/* First kill our subnodes: */
			if (valid(rec->left))
				killAll(rec->left);

			if (valid(rec->right))
				killAll(rec->right);

			if (valid(rec->parent)) {                /* We're not root. */
				if (rec->parent->left == rec)
					rec->parent->left = nullNode;
				else
					rec->parent->right = nullNode;
			}

			Dealloc(rec->id);
			rec->left = NULL;
			rec->right = NULL;
			delete rec;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::killAll()
		{
			killAll(rootNode);
			rootNode = nullNode;
			m_cachedSize = 0;
		}

		template <class Key, class Data>
		size_t RedBlackTree<Key, Data>::mem_usage() const
		{
			size_t ret = sizeof(*this);
			if (!valid(rootNode)) return ret;

			ret += rootNode->mem_usage(this);
			return ret;
		}

		template <class Key, class Data>
		DArray<Data> *RedBlackTree<Key, Data>::ConvertToDArray() const
		{
			DArray<Data> *darray = new DArray<Data> ((int)size());
			RecursiveConvertToDArray(darray, rootNode);
			return darray;
		}

		template <class Key, class Data>
		DArray<Key> *RedBlackTree<Key, Data>::ConvertIndexToDArray() const
		{
			DArray<Key> *darray = new DArray<Key> ((int)size());
			RecursiveConvertIndexToDArray(darray, rootNode);
			return darray;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::RecursiveConvertToDArray(DArray<Data> *darray, RedBlackNode<Key, Data> *btree) const
		{
			if (!valid(btree)) return;

			RecursiveConvertToDArray(darray, btree->left);
			darray->insert(btree->data);
			RecursiveConvertToDArray(darray, btree->right);
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::RecursiveConvertIndexToDArray(DArray<Key> *darray, RedBlackNode<Key, Data> *btree) const
		{
			if (!valid(btree)) return;

			RecursiveConvertIndexToDArray(darray, btree->left);
			darray->insert(btree->id);
			RecursiveConvertIndexToDArray(darray, btree->right);
		}
	}
}
