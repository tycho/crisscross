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

#ifndef __included_cc_stree_h
#error "This file shouldn't be compiled directly."
#endif

namespace CrissCross
{
	namespace Data
	{
		template <class Key, class Data, bool OwnsKeys>
		STree<Key, Data, OwnsKeys>::STree()
		{
			m_root = nullptr;
			m_size = 0;
		}

		template <class Key, class Data, bool OwnsKeys>
		STree<Key, Data, OwnsKeys>::~STree()
		{
			if (m_root)
				Dealloc(m_root->id);

			delete m_root;
			m_root = nullptr;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool STree<Key, Data, OwnsKeys>::erase(Key const &_key)
		{
			SNode<Key, Data, OwnsKeys> *node = findNode(_key),
				**ploc = nullptr;

			if (!valid(node))
				return false;

			if (node->parent) {
				int pos = Compare(node->id, node->parent->id);
				ploc = (pos < 0) ?
					&node->parent->left :
					&node->parent->right;
			} else {
				ploc = &m_root;
			}

			--m_size;
			Dealloc(node->id);

			if (node->left && node->right) {
				SNode<Key, Data, OwnsKeys> *min = node->right;
				while(min->left)
					min = min->left;
				node->id = min->id;
				node->data = min->data;
				min->parent->left = min->right;
				node = min;
			} else if (node->left) {
				node->left->parent = node->parent;
				*ploc = node->left;
			} else if (node->right) {
				node->right->parent = node->parent;
				*ploc = node->right;
			}

			node->left = nullptr;
			node->right = nullptr;

			if (m_root == node)
				m_root = nullptr;

			delete node;

			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool STree<Key, Data, OwnsKeys>::replace(Key const &key, Data const &_data)
		{
			SNode<Key, Data, OwnsKeys> *current = findNode(key);
			if (!valid(current))
				return false;
			current->data = _data;
			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		bool STree<Key, Data, OwnsKeys>::insert(Key const &_key, Data const &_data)
		{
			SNode<Key, Data, OwnsKeys> *parent = m_root;
			int cmp = 0;
			while (valid(parent)) {
				cmp = Compare(_key, parent->id);
				if (cmp < 0) {
					if (!parent->left)
						break;
					parent = parent->left;
				} else if (cmp > 0) {
					if (!parent->right)
						break;
					parent = parent->right;
				} else {
					return false;
				}
			}

			SNode<Key, Data, OwnsKeys> *newnode = new SNode<Key, Data, OwnsKeys>();
			newnode->id = Duplicate(_key);
			newnode->data = _data;
			newnode->parent = parent;

			if (valid(parent)) {
				if (cmp < 0)
					parent->left = newnode;
				else if (cmp > 0)
					parent->right = newnode;
				else
					/* Now that's just -nonsense- */
					return false;
			} else {
				m_root = newnode;
			}

			++m_size;
			return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		SNode<Key, Data, OwnsKeys> *STree<Key, Data, OwnsKeys>::findNode(Key const &_key) const
		{
			SNode<Key, Data, OwnsKeys> *p_current = m_root;
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
		TypedData STree<Key, Data, OwnsKeys>::find(Key const &_key, TypedData const &_default) const
		{
			SNode<Key, Data, OwnsKeys> *p_current = findNode(_key);

			if (!p_current)
				return _default;

			return (TypedData)(p_current->data);
		}

		template <class Key, class Data, bool OwnsKeys>
		bool STree<Key, Data, OwnsKeys>::exists(Key const &_key) const
		{
			SNode<Key, Data, OwnsKeys> *p_current = findNode(_key);
			if (!p_current) return false;
			else return true;
		}

		template <class Key, class Data, bool OwnsKeys>
		size_t STree<Key, Data, OwnsKeys>::mem_usage() const
		{
			size_t ret = sizeof(*this);
			if (!m_root) return ret;

			ret += m_root->mem_usage();
			return ret;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		DArray<TypedData> *STree<Key, Data, OwnsKeys>::ConvertToDArray() const
		{
			DArray<TypedData> *darray = new DArray<TypedData>();
			darray->setSize(m_size);
			RecursiveConvertToDArray(darray, m_root);
			return darray;
		}

		template <class Key, class Data, bool OwnsKeys>
		DArray<Key> *STree<Key, Data, OwnsKeys>::ConvertIndexToDArray() const
		{
			DArray<Key> *darray = new DArray<Key>();
			darray->setSize(m_size);
			RecursiveConvertIndexToDArray(darray, m_root);
			return darray;
		}

		template <class Key, class Data, bool OwnsKeys>
		template <class TypedData>
		void STree<Key, Data, OwnsKeys>::RecursiveConvertToDArray(DArray<TypedData> *darray, SNode<Key, Data, OwnsKeys> *btree) const
		{
			if (!btree) return;

			RecursiveConvertToDArray(darray, btree->left);
			darray->insert((TypedData)(btree->data));
			RecursiveConvertToDArray(darray, btree->right);
		}

		template <class Key, class Data, bool OwnsKeys>
		void STree<Key, Data, OwnsKeys>::RecursiveConvertIndexToDArray(DArray<Key> *darray, SNode<Key, Data, OwnsKeys> *btree) const
		{
			if (!btree) return;

			RecursiveConvertIndexToDArray(darray, btree->left);
			darray->insert(btree->id);
			RecursiveConvertIndexToDArray(darray, btree->right);
		}
	}
}
