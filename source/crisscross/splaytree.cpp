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
#error "This file shouldn't be compiled directly."
#endif

#include <stdlib.h>

#include <crisscross/internal_mem.h>

namespace CrissCross
{
	namespace Data
	{
		template <class Key, class Data, bool OwnsKeys>
		SplayTree<Key, Data, OwnsKeys>::SplayTree()
		{
			root = NULL;
			m_size = 0;
		}

		template <class Key, class Data, bool OwnsKeys>
		SplayTree<Key, Data, OwnsKeys>::~SplayTree()
		{
			while (m_size > 0)
				erase(root->id);
		}

		template <class Key, class Data, bool OwnsKeys>
		bool SplayTree<Key, Data, OwnsKeys>::insert(Key const &key, Data const &x)
		{
			static SplayNode<Key, Data, OwnsKeys> *newNode = NULL;

			if (newNode == NULL)
				newNode = new SplayNode<Key, Data, OwnsKeys>;
			else if (OwnsKeys)
				Dealloc(newNode->id);

			if (OwnsKeys)
				newNode->id = Duplicate(key);
			else
				newNode->id = key;
			newNode->data = x;
			newNode->parent = newNode->right = newNode->left = NULL;

			if (root == NULL) {
				newNode->left = newNode->right = NULL;
				root = newNode;
			} else {
				splay(key, root);
				if (Compare(key, root->id) < 0)	{
					newNode->left = root->left;
					if (newNode->left)
						newNode->left->parent = newNode;

					newNode->right = root;
					root->parent = newNode;
					root->left = NULL;
					root = newNode;
				} else
				if (Compare(root->id, key) < 0)	{
					newNode->right = root->right;
					if (newNode->right)
						newNode->right->parent = newNode;

					newNode->left = root;
					root->parent = newNode;
					root->right = NULL;
					root = newNode;
				} else
					return false;
			}

			m_size++;
			newNode = NULL;               /* So next insert will call new */
			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool SplayTree<Key, Data, OwnsKeys>::killNode(SplayNode<Key, Data, OwnsKeys> * z)
		{
			SplayNode<Key, Data, OwnsKeys> *x, *y;

			if (z->left == NULL || z->right == NULL) {
				/* y has a NULL node as a child */
				y = z;
			} else {
				/* find tree successor with a NULL node as a child */
				y = z->right;

				while (y->left != NULL)
					y = y->left;
			}

			/* x is y's only child */
			if (y->left != NULL)
				x = y->left;
			else
				x = y->right;

			/* remove y from the parent chain */
			if (x) x->parent = y->parent;

			if (y->parent) {
				if (y == y->parent->left)
					y->parent->left = x;
				else
					y->parent->right = x;
			} else
				root = x;

			if (y != z) {
				if ( OwnsKeys ) {
					Dealloc( z->id );
					z->id = Duplicate( y->id );
				} else {
					z->id = y->id;
				}
				z->data = y->data;
			} else if (OwnsKeys) {
				Dealloc(y->id);
			}

			m_size--;

			y->left = NULL;
			y->right = NULL;
			delete y;

			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool SplayTree<Key, Data, OwnsKeys>::erase(Key const &key)
		{
			SplayNode<Key, Data, OwnsKeys> *newTree;

			if (!root) return false;

			/* If key is found, it will be at the root */
			splay(key, root);
			if (Compare(root->id, key) != 0)
				return false;

			if (root == NULL)
				return false;

			if (root->left == NULL)
				newTree = root->right;
			else{
				/* Find the maximum in the left subtree */
				/* Splay it to the root; and then attach right child */
				newTree = root->left;
				splay(key, newTree);
				newTree->right = root->right;
			}

			m_size--;

			root->left = NULL;
			root->right = NULL;
			delete root;

			root = newTree;

			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool SplayTree<Key, Data, OwnsKeys>::exists(Key const &key) const
		{
			splay(key, root);

			if (root == NULL || Compare(root->id, key) != 0)
				return false;

			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		TypedData SplayTree<Key, Data, OwnsKeys>::find(Key const &key, TypedData const &_default) const
		{
			SplayNode<Key, Data, OwnsKeys> *node = findNode(key);

			if (!node)
				return _default;

			return (TypedData)(root->data);
		}

		template <class Key, class Data, bool OwnsKeys>
		bool SplayTree<Key, Data, OwnsKeys>::replace(Key const &key, Data const &data)
		{
			splay(key, root);

			if (root == NULL || Compare(root->id, key) != 0)
				return false;

			root->data = data;
			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		SplayNode<Key, Data, OwnsKeys> *SplayTree<Key, Data, OwnsKeys>::findNode(Key const &key) const
		{
			splay(key, root);

			if (root == NULL || Compare(root->id, key) != 0)
				return NULL;

			return root;
		}

		template <class Key, class Data, bool OwnsKeys>
		void SplayTree<Key, Data, OwnsKeys>::empty()
		{
			while (m_size > 0)
				erase(root->id);
		}

		template <class Key, class Data, bool OwnsKeys>
		void SplayTree<Key, Data, OwnsKeys>::splay(Key const &key, SplayNode<Key, Data, OwnsKeys> * & t) const
		{
			if (!t) return;

			SplayNode<Key, Data, OwnsKeys> *leftTreeMax, *rightTreeMin;
			static SplayNode<Key, Data, OwnsKeys> header;

			header.left = header.right = NULL;

			leftTreeMax = rightTreeMin = &header;

			for ( ; ;)
				if (Compare(key, t->id) < 0) {
					if (t->left == NULL)
						break;

					if (Compare(key, t->left->id) < 0)
						rotateWithLeftChild(t);

					if (t->left == NULL)
						break;

					/* Link Right */
					rightTreeMin->left = t;
					rightTreeMin = t;
					t = t->left;
				} else if (Compare(t->id, key) < 0) {
					if (t->right == NULL)
						break;

					if (Compare(t->right->id, key) < 0)
						rotateWithRightChild(t);

					if (t->right == NULL)
						break;

					/* Link Left */
					leftTreeMax->right = t;
					leftTreeMax = t;
					t = t->right;
				} else
					break;

			leftTreeMax->right = t->left;
			rightTreeMin->left = t->right;
			t->left = header.right;
			t->right = header.left;

			header.left = header.right = NULL;
		}


		template <class Key, class Data, bool OwnsKeys>
		void SplayTree<Key, Data, OwnsKeys>::rotateWithLeftChild(SplayNode<Key, Data, OwnsKeys> * & k2) const
		{
			SplayNode<Key, Data, OwnsKeys> *k1 = k2->left;
			k2->left = k1->right;
			if (k2->left)
				k2->left->parent = k2;

			k1->right = k2;
			k1->parent = k2->parent;
			k2->parent = k1;
			k2 = k1;
		}

		template <class Key, class Data, bool OwnsKeys>
		void SplayTree<Key, Data, OwnsKeys>::rotateWithRightChild(SplayNode<Key, Data, OwnsKeys> * & k1) const
		{
			SplayNode<Key, Data, OwnsKeys> *k2 = k1->right;
			k1->right = k2->left;
			if (k1->right)
				k1->right->parent = k1;

			k2->left = k1;
			k2->parent = k1->parent;
			k1->parent = k2;
			k1 = k2;
		}

		template <class Key, class Data, bool OwnsKeys>
		size_t SplayTree<Key, Data, OwnsKeys>::mem_usage() const
		{
			size_t ret = sizeof(*this);
			if (!root) return ret;

			ret += root->mem_usage();
			return ret;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		DArray<TypedData> *SplayTree<Key, Data, OwnsKeys>::ConvertToDArray() const
		{
			DArray<TypedData> *darray = new DArray<TypedData> ((int)size());
			RecursiveConvertToDArray<TypedData>(darray, root);
			return darray;
		}

		template <class Key, class Data, bool OwnsKeys>
		DArray<Key> *SplayTree<Key, Data, OwnsKeys>::ConvertIndexToDArray() const
		{
			DArray<Key> *darray = new DArray<Key> ((int)size());
			RecursiveConvertIndexToDArray(darray, root);
			return darray;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		void SplayTree<Key, Data, OwnsKeys>::RecursiveConvertToDArray(DArray<TypedData> *darray, SplayNode<Key, Data, OwnsKeys> *btree) const
		{
			if (!btree) return;

			RecursiveConvertToDArray(darray, btree->left);
			darray->insert((TypedData)(btree->data));
			RecursiveConvertToDArray(darray, btree->right);
		}

		template <class Key, class Data, bool OwnsKeys>
		void SplayTree<Key, Data, OwnsKeys>::RecursiveConvertIndexToDArray(DArray<Key> *darray, SplayNode<Key, Data, OwnsKeys> *btree) const
		{
			if (!btree) return;

			RecursiveConvertIndexToDArray(darray, btree->left);
			darray->insert(btree->id);
			RecursiveConvertIndexToDArray(darray, btree->right);
		}
	}
}
