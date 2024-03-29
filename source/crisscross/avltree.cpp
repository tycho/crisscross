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
#error "This file shouldn't be compiled directly."
#endif

namespace CrissCross
{
	namespace Data
	{
		template <class Key, class Data, bool OwnsKeys>
		AVLTree<Key, Data, OwnsKeys>::AVLTree()
		{
			m_root = nullptr;
			m_size = 0;
		}

		template <class Key, class Data, bool OwnsKeys>
		AVLTree<Key, Data, OwnsKeys>::~AVLTree()
		{
			if (m_root && OwnsKeys)
				Dealloc(m_root->id);

			delete m_root;
			m_root = nullptr;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool AVLTree<Key, Data, OwnsKeys>::erase(Key const &_key)
		{
			int ret = erase(&m_root, _key);
			return (ret == OK || ret == BALANCE);
		}

		template <class Key, class Data, bool OwnsKeys>
		bool AVLTree<Key, Data, OwnsKeys>::replace(Key const &key, Data const &_data)
		{
			AVLNode<Key, Data, OwnsKeys> *current;
			current = findNode(key);
			if (!valid(current)) return false;

			current->data = _data;
			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		int AVLTree<Key, Data, OwnsKeys>::erase(AVLNode<Key, Data, OwnsKeys> **_node, Key const &_key)
		{
			if (!*_node)
				return INVALID;

			int result = OK;

			if (Compare(_key, (*_node)->id) != 0) {
				if (Compare(_key, (*_node)->id) < 0) {
					if ((*_node)->left) {
						if ((result = erase(&(*_node)->left, _key)) == BALANCE)
							return balanceLeftShrunk(_node);

						if (result != INVALID)
							return result;
					}
				} else if ((*_node)->right) {
					if ((result = erase(&(*_node)->right, _key)) == BALANCE)
						return balanceRightShrunk(_node);

					if (result != INVALID)
						return result;
				}

				/* Node not found */
				return INVALID;
			}

			/* Erase this node */
			--m_size;
			if (OwnsKeys)
				Dealloc((*_node)->id);

			if ((*_node)->left) {
				if (replaceWithHighest(*_node, &(*_node)->left, &result)) {
					if (result == BALANCE)
						result = balanceLeftShrunk(_node);

					return result;
				}
			}

			if ((*_node)->right) {
				if (replaceWithLowest(*_node, &(*_node)->right, &result)) {
					if (result == BALANCE)
						result = balanceRightShrunk(_node);

					return result;
				}
			}

			(*_node)->left = nullptr;
			(*_node)->right = nullptr;

			/* Leaf, delete and rebalance */
			delete *_node, *_node = 0;

			return BALANCE;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool AVLTree<Key, Data, OwnsKeys>::insert(Key const &_key, Data const &_data)
		{
			if (insert(nullptr, &m_root, _key, _data) != INVALID) {
				++m_size;
				return true;
			} else
				return false;
		}

		template <class Key, class Data, bool OwnsKeys>
		AVLNode<Key, Data, OwnsKeys> *AVLTree<Key, Data, OwnsKeys>::findNode(Key const &_key) const
		{
			AVLNode<Key, Data, OwnsKeys> *p_current = m_root;
			while (p_current) {
				int cmp = Compare(_key, p_current->id);
				if (cmp < 0)
					p_current = p_current->left;
				else if (cmp > 0)
					p_current = p_current->right;
				else {
					return p_current;
				}
			}
			return nullptr;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		TypedData AVLTree<Key, Data, OwnsKeys>::find(Key const &_key, TypedData const &_default) const
		{
			AVLNode<Key, Data, OwnsKeys> *p_current = findNode(_key);

			if (!p_current)
				return _default;

			return (TypedData)(p_current->data);
		}

		template <class Key, class Data, bool OwnsKeys>
		bool AVLTree<Key, Data, OwnsKeys>::exists(Key const &_key) const
		{
			AVLNode<Key, Data, OwnsKeys> *p_current = findNode(_key);
			if (!p_current) return false;
			else return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		void AVLTree<Key, Data, OwnsKeys>::rotateLeft(AVLNode<Key, Data, OwnsKeys> **_node)
		{
			AVLNode<Key, Data, OwnsKeys> *p_tmp = *_node;
			*_node = p_tmp->right;
			p_tmp->right = (*_node)->left;
			(*_node)->left = p_tmp;
			(*_node)->parent = p_tmp->parent;
			p_tmp->parent = *_node;
			if (p_tmp->right)
				p_tmp->right->parent = (*_node)->left;
		}

		template <class Key, class Data, bool OwnsKeys>
		void AVLTree<Key, Data, OwnsKeys>::rotateRight(AVLNode<Key, Data, OwnsKeys> **_node)
		{
			AVLNode<Key, Data, OwnsKeys> *p_tmp = *_node;
			*_node = p_tmp->left;
			p_tmp->left = (*_node)->right;
			(*_node)->right = p_tmp;

			(*_node)->parent = p_tmp->parent;
			p_tmp->parent = *_node;
			if (p_tmp->left)
				p_tmp->left->parent = (*_node)->right;
		}

		template <class Key, class Data, bool OwnsKeys>
		int AVLTree<Key, Data, OwnsKeys>::balanceLeftGrown(AVLNode<Key, Data, OwnsKeys> **_node)
		{
			switch ((*_node)->balance)
			{
			case LEFTHEAVY:
			{
				if ((*_node)->left->balance == LEFTHEAVY) {
					(*_node)->balance = (*_node)->left->balance = BALANCED;
					rotateRight(_node);
				} else {
					switch ((*_node)->left->right->balance)
					{
					case LEFTHEAVY:
					{
						(*_node)->balance = RIGHTHEAVY;
						(*_node)->left->balance = BALANCED;
						break;
					}

					case BALANCED:
					{
						(*_node)->balance = BALANCED;
						(*_node)->left->balance = BALANCED;
						break;
					}

					case RIGHTHEAVY:
					{
						(*_node)->balance = BALANCED;
						(*_node)->left->balance = LEFTHEAVY;
						break;
					}
					}

					(*_node)->left->right->balance = BALANCED;

					rotateLeft(&(*_node)->left);
					rotateRight(_node);
				}

				return OK;
			}

			case BALANCED:
			{
				(*_node)->balance = LEFTHEAVY;
				return BALANCE;
			}

			case RIGHTHEAVY:
			{
				(*_node)->balance = BALANCED;
				return OK;
			}
			}

			return INVALID;
		}

		template <class Key, class Data, bool OwnsKeys>
		int AVLTree<Key, Data, OwnsKeys>::balanceRightGrown(AVLNode<Key, Data, OwnsKeys> **_node)
		{
			switch ((*_node)->balance)
			{
			case LEFTHEAVY:
			{
				(*_node)->balance = BALANCED;
				return OK;
			}

			case BALANCED:
			{
				(*_node)->balance = RIGHTHEAVY;
				return BALANCE;
			}

			case RIGHTHEAVY:
			{
				if ((*_node)->right->balance == RIGHTHEAVY) {
					(*_node)->balance = (*_node)->right->balance = BALANCED;
					rotateLeft(_node);
				} else {
					switch ((*_node)->right->left->balance)
					{
					case LEFTHEAVY:
					{
						(*_node)->balance = BALANCED;
						(*_node)->right->balance = RIGHTHEAVY;
						break;
					}

					case BALANCED:
					{
						(*_node)->balance = BALANCED;
						(*_node)->right->balance = BALANCED;
						break;
					}

					case RIGHTHEAVY:
					{
						(*_node)->balance = LEFTHEAVY;
						(*_node)->right->balance = BALANCED;
						break;
					}
					}

					(*_node)->right->left->balance = BALANCED;

					rotateRight(&(*_node)->right);
					rotateLeft(_node);
				}

				return OK;
			}
			}

			return INVALID;
		}

		template <class Key, class Data, bool OwnsKeys>
		int AVLTree<Key, Data, OwnsKeys>::balanceLeftShrunk(AVLNode<Key, Data, OwnsKeys> **_node)
		{
			switch ((*_node)->balance)
			{
			case LEFTHEAVY:
			{
				(*_node)->balance = BALANCED;
				return BALANCE;
			}

			case BALANCED:
			{
				(*_node)->balance = RIGHTHEAVY;
				return OK;
			}

			case RIGHTHEAVY:
			{
				switch ((*_node)->right->balance)
				{
				case LEFTHEAVY:
				{
					switch ((*_node)->right->left->balance)
					{
					case LEFTHEAVY:
					{
						(*_node)->balance = BALANCED;
						(*_node)->right->balance = RIGHTHEAVY;
						break;
					}

					case BALANCED:
					{
						(*_node)->balance = BALANCED;
						(*_node)->right->balance = BALANCED;
						break;
					}

					case RIGHTHEAVY:
					{
						(*_node)->balance = LEFTHEAVY;
						(*_node)->right->balance = BALANCED;
						break;
					}
					}

					(*_node)->right->left->balance = BALANCED;

					rotateRight(&(*_node)->right);
					rotateLeft(_node);

					return BALANCE;
				}

				case BALANCED:
				{
					(*_node)->balance = RIGHTHEAVY;
					(*_node)->right->balance = LEFTHEAVY;
					rotateLeft(_node);
					return OK;
				}

				case RIGHTHEAVY:
				{
					(*_node)->balance = (*_node)->right->balance = BALANCED;
					rotateLeft(_node);
					return BALANCE;
				}
				}

				break;
			}
			}

			return INVALID;
		}

		template <class Key, class Data, bool OwnsKeys>
		int AVLTree<Key, Data, OwnsKeys>::balanceRightShrunk(AVLNode<Key, Data, OwnsKeys> **_node)
		{
			switch ((*_node)->balance)
			{
			case LEFTHEAVY:
			{
				switch ((*_node)->left->balance)
				{
				case LEFTHEAVY:
				{
					(*_node)->balance = (*_node)->left->balance = BALANCED;
					rotateRight(_node);
					return BALANCE;
				}

				case BALANCED:
				{
					(*_node)->balance = LEFTHEAVY;
					(*_node)->left->balance = RIGHTHEAVY;
					rotateRight(_node);
					return OK;
				}

				case RIGHTHEAVY:
				{
					switch ((*_node)->left->right->balance)
					{
					case LEFTHEAVY:
					{
						(*_node)->balance = RIGHTHEAVY;
						(*_node)->left->balance = BALANCED;
						break;
					}

					case BALANCED:
					{
						(*_node)->balance = BALANCED;
						(*_node)->left->balance = BALANCED;
						break;
					}

					case RIGHTHEAVY:
					{
						(*_node)->balance = BALANCED;
						(*_node)->left->balance = LEFTHEAVY;
						break;
					}
					}

					(*_node)->left->right->balance = BALANCED;

					rotateLeft(&(*_node)->left);
					rotateRight(_node);

					return BALANCE;
				}
				}

				break;
			}

			case BALANCED:
			{
				(*_node)->balance = LEFTHEAVY;
				return OK;
			}

			case RIGHTHEAVY:
			{
				(*_node)->balance = BALANCED;
				return BALANCE;
			}
			}

			return INVALID;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool AVLTree<Key, Data, OwnsKeys>::replaceWithHighest(AVLNode<Key, Data, OwnsKeys> * _target, AVLNode<Key, Data, OwnsKeys> **_subtree, int * _result)
		{
			AVLNode<Key, Data, OwnsKeys> *p_tmp;

			*_result = BALANCE;

			if (!*_subtree)
				return false;

			if ((*_subtree)->right) {
				if (!replaceWithHighest(_target, &(*_subtree)->right, _result))
					return false;

				if (*_result == BALANCE)
					*_result = balanceRightShrunk(_subtree);

				return true;
			}

			_target->data = (*_subtree)->data;
			if (OwnsKeys)
				_target->id = Duplicate((*_subtree)->id);
			else
				_target->id = (*_subtree)->id;

			p_tmp = *_subtree;
			*_subtree = p_tmp->left;
			if (*_subtree)
				(*_subtree)->parent = p_tmp->parent;

			if (p_tmp) {
				if (OwnsKeys)
					Dealloc(p_tmp->id);
				p_tmp->left = nullptr;
				p_tmp->right = nullptr;
				delete p_tmp;
			}

			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool AVLTree<Key, Data, OwnsKeys>::replaceWithLowest(AVLNode<Key, Data, OwnsKeys> * _target, AVLNode<Key, Data, OwnsKeys> **_subtree, int * _result)
		{
			AVLNode<Key, Data, OwnsKeys> *p_tmp;

			*_result = BALANCE;

			if (!*_subtree)
				return false;

			if ((*_subtree)->left) {
				if (!replaceWithLowest(_target, &(*_subtree)->left, _result))
					return false;

				if (*_result == BALANCE)
					*_result = balanceLeftShrunk(_subtree);

				return true;
			}

			_target->data = (*_subtree)->data;
			if (OwnsKeys)
				_target->id = Duplicate((*_subtree)->id);
			else
				_target->id = (*_subtree)->id;

			p_tmp = *_subtree;
			*_subtree = p_tmp->right;
			if (*_subtree)
				(*_subtree)->parent = p_tmp->parent;

			if (p_tmp) {
				if (OwnsKeys)
					Dealloc(p_tmp->id);
				p_tmp->left = nullptr;
				p_tmp->right = nullptr;
				delete p_tmp;
			}

			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		int AVLTree<Key, Data, OwnsKeys>::insert(AVLNode<Key, Data, OwnsKeys> **pp_parent, AVLNode<Key, Data, OwnsKeys> **_node, Key const &_key, Data const &_data)
		{
			int result = OK;

			CoreAssert(_node);

			if (!*_node) {
				*_node = new AVLNode<Key, Data, OwnsKeys>();
				(*_node)->parent = pp_parent ? *pp_parent : nullptr;
				if (OwnsKeys)
					(*_node)->id = Duplicate(_key);
				else
					(*_node)->id = _key;
				(*_node)->data = _data;
				return BALANCE;
			}

			int ret = Compare(_key, (*_node)->id);
			if (ret < 0) {
				if ((result = insert(_node, &(*_node)->left, _key, _data)) == BALANCE)
					result = balanceLeftGrown(_node);
			} else if (ret > 0) {           /* obj >= nodeobj */
				if ((result = insert(_node, &(*_node)->right, _key, _data)) == BALANCE)
					result = balanceRightGrown(_node);
			} else {
				return INVALID;
			}

			return result;
		}

		template <class Key, class Data, bool OwnsKeys>
		uint32_t AVLTree<Key, Data, OwnsKeys>::mem_usage() const
		{
			uint32_t ret = sizeof(*this);
			if (!m_root) return ret;

			ret += m_root->mem_usage();
			return ret;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		DArray<TypedData> *AVLTree<Key, Data, OwnsKeys>::ConvertToDArray() const
		{
			DArray<TypedData> *darray = new DArray<TypedData>();
			darray->setSize(m_size);
			RecursiveConvertToDArray<TypedData>(darray, m_root);
			return darray;
		}

		template <class Key, class Data, bool OwnsKeys>
		DArray<Key> *AVLTree<Key, Data, OwnsKeys>::ConvertIndexToDArray() const
		{
			DArray<Key> *darray = new DArray<Key>();
			darray->setSize(m_size);
			RecursiveConvertIndexToDArray(darray, m_root);
			return darray;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		void AVLTree<Key, Data, OwnsKeys>::RecursiveConvertToDArray(DArray<TypedData> *darray, AVLNode<Key, Data, OwnsKeys> *btree) const
		{
			if (!btree) return;

			RecursiveConvertToDArray(darray, btree->left);
			darray->insert(reinterpret_cast<TypedData>(btree->data));
			RecursiveConvertToDArray(darray, btree->right);
		}

		template <class Key, class Data, bool OwnsKeys>
		void AVLTree<Key, Data, OwnsKeys>::RecursiveConvertIndexToDArray(DArray<Key> *darray, AVLNode<Key, Data, OwnsKeys> *btree) const
		{
			if (!btree) return;

			RecursiveConvertIndexToDArray(darray, btree->left);
			darray->insert(btree->id);
			RecursiveConvertIndexToDArray(darray, btree->right);
		}
	}
}
