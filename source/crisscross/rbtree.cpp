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
			nullNode->data_ind = nullNode->id_ind = keyPool.insert(NullKey<Key>());
			dataPool.insert(NullKey<Data>(), nullNode->id_ind);
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
			dataPool.insert(rec, current->data_ind);
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
				current = (Compare(key, keyPool[current->id_ind]) <= 0) ?
				          current->left : current->right;
			}

			/* setup new node */
			if ((x = new RedBlackNode<Key, Data>()) == 0) {
				return false;
			}

			x->parent = parent;
			x->left = nullNode;
			x->right = nullNode;
			x->color = RED;
			x->data_ind = x->id_ind = keyPool.insert(Duplicate(key));
			dataPool.insert(rec, x->id_ind);

			/* insert node in tree */
			if (valid(parent)) {
				if (Compare(key, keyPool[parent->id_ind]) <= 0)
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
				if (Compare(key, keyPool[z->id_ind]) == 0)
					break;
				else{
					z = (Compare(key, keyPool[z->id_ind]) <= 0) ? z->left : z->right;
				}
			}

			if (z == nullNode) {
				return false;
			}

			return killNode(z);
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::erase(Key const &key, Data const &rec)
		{
			RedBlackNode<Key, Data> *node = findNode(key);
			return erase(key, rec, node);
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::erase(Key const &key, Data const &rec, RedBlackNode<Key, Data> *curnode)
		{
			if (!valid(curnode)) return false;

			bool killed = false;

			if (Compare(keyPool[curnode->id_ind], key) == 0) {
				if (Compare(dataPool[curnode->data_ind], rec) == 0) {
					killNode(curnode);
					killed = true;
				}

				if (!killed)
					killed = erase(key, rec, curnode->left);

				if (!killed)
					killed = erase(key, rec, curnode->right);
			}

			return killed;
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
				Dealloc(keyPool[z->id_ind]);
				keyPool[z->id_ind] = Duplicate(keyPool[y->id_ind]);
				dataPool[z->data_ind] = dataPool[y->data_ind];
			} else {
				Dealloc(keyPool[y->id_ind]);
			}

			if (y->color == BLACK)
				deleteFixup(x);

			m_cachedSize--;

			keyPool.remove(y->id_ind);
			dataPool.remove(y->data_ind);
			y->left = NULL;
			y->right = NULL;
			delete y;

			return true;
		}

		template <class Key, class Data>
		Data RedBlackTree<Key, Data>::find(Key const &key) const
		{
			RedBlackNode<Key, Data> *current = rootNode;

			while (valid(current)) {
				if (Compare(key, keyPool[current->id_ind]) == 0) {
					return dataPool[current->data_ind];
				} else {
					current = (Compare(key, keyPool[current->id_ind]) <= 0) ?
					          current->left : current->right;
				}
			}

			return NULL;
		}

		template <class Key, class Data>
		bool RedBlackTree<Key, Data>::find(Key const &key, Data &_data) const
		{
			RedBlackNode<Key, Data> *current = rootNode;

			while (valid(current)) {
				if (Compare(key, keyPool[current->id_ind]) == 0) {
					_data = dataPool[current->data_ind];
					return true;
				} else {
					current = (Compare(key, keyPool[current->id_ind]) <= 0) ?
					          current->left : current->right;
				}
			}

			return false;
		}

		template <class Key, class Data>
		RedBlackNode<Key, Data> * RedBlackTree<Key, Data>::findNode(Key const &key) const
		{
			RedBlackNode<Key, Data> * current = rootNode;

			while (valid(current)) {
				if (Compare(key, keyPool[current->id_ind]) == 0) {
					return current;
				} else {
					current = (Compare(key, keyPool[current->id_ind]) <= 0) ?
						current->left : current->right;
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

			Dealloc(keyPool[rec->id_ind]);
			keyPool.remove(rec->id_ind);
			dataPool.remove(rec->data_ind);
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
		DArray<Data> *RedBlackTree<Key, Data>::findAll(Key const &_key) const
		{
			RedBlackNode<Key, Data> *p_current = findNode(_key);
			DArray<Data>            *data = new DArray<Data>();
			findRecursive(data, _key, p_current);
			return data;
		}

		template <class Key, class Data>
		void RedBlackTree<Key, Data>::findRecursive(DArray<Data> *_array, Key const &_key, RedBlackNode<Key, Data> *_node) const
		{
			CoreAssert(_array);
			if (!valid(_node)) return;

			findRecursive(_array, _key, _node->left);
			if (Compare(keyPool[_node->id_ind], _key) == 0) {
				_array->insert(dataPool[_node->data_ind]);
			}

			findRecursive(_array, _key, _node->right);
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
		DArray<Data> *RedBlackTree<Key, Data>::ConvertToDArray(bool _encapsulate) const
		{
			DArray<Data> *darray = new DArray<Data>(dataPool, _encapsulate);
			return darray;
		}

		template <class Key, class Data>
		DArray<Key> *RedBlackTree<Key, Data>::ConvertIndexToDArray(bool _encapsulate) const
		{
			DArray<Key> *darray = new DArray<Key>(keyPool, _encapsulate);
			return darray;
		}
	}
}
