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
#error "This file shouldn't be compiled directly."
#endif

namespace CrissCross
{
	namespace Data
	{
		template <class Key, class Data>
		VTree<Key, Data>::VTree()
		{
			m_size = 0;
		}

		template <class Key, class Data>
		VTree<Key, Data>::~VTree()
		{
			m_root.reset();
		}

		template <class Key, class Data>
		bool VTree<Key, Data>::erase(Key const &_key)
		{
			cow_ptr<VNode<Key, Data>> node = findNode(_key),
				*ploc;

			if (!valid(node))
				return false;

			if (valid(node->parent)) {
				int pos = Compare(node->id, node->parent->id);
				ploc = (pos < 0) ?
					&node->parent->left :
					&node->parent->right;
			} else {
				ploc = &m_root;
			}

			--m_size;
			Dealloc(node->id);

			if (has_left_child(node) && has_right_child(node)) {
				cow_ptr<VNode<Key, Data>> min = node.get()->right;
				while(has_left_child(min))
					min = min.get()->left;
				node->id = min->id;
				node->data = min->data;
				min->parent->left = min->right;
				node = min;
			} else if (has_left_child(node)) {
				node->left->parent = node->parent;
				*ploc = node->left;
			} else if (has_right_child(node)) {
				node->right->parent = node->parent;
				*ploc = node->right;
			}

			node.get()->left.reset();
			node.get()->right.reset();

			if (m_root == node)
				m_root.reset();

			return true;
		}

		template <class Key, class Data>
		bool VTree<Key, Data>::replace(Key const &key, Data const &_data)
		{
			VNode<Key, Data> *current = findNode(key);
			if (!valid(current))
				return false;
			current->data = _data;
			return true;
		}

		template <class Key, class Data>
		bool VTree<Key, Data>::insert(Key const &_key, Data const &_data)
		{
			cow_ptr<VNode<Key, Data>> parent = m_root;
			int cmp;
			while (valid(parent)) {
				cmp = Compare<Key>(_key, parent->id);
				if (cmp < 0) {
					if (!has_left_child(parent))
						break;
					parent = parent->left;
				} else if (cmp > 0) {
					if (!has_right_child(parent))
						break;
					parent = parent->right;
				} else {
					return false;
				}
			}

			cow_ptr<VNode<Key, Data>> newnode(new VNode<Key, Data>(_key, _data));
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

		template <class Key, class Data>
		cow_ptr<VNode<Key, Data>> VTree<Key, Data>::findNode(Key const &_key) const
		{
			cow_ptr<VNode<Key, Data>> current = m_root;
			while (current.get()) {
				int cmp = Compare(_key, current.get()->id);
				if (cmp < 0)
					current = current.get()->left;
				else if (cmp > 0)
					current = current.get()->right;
				else {
					return current;
				}
			}
			return cow_ptr<VNode<Key, Data>>();
		}

		template <class Key, class Data>
		Data VTree<Key, Data>::find(Key const &_key, Data const &_default) const
		{
			cow_ptr<VNode<Key, Data>> current = findNode(_key);

			if (!current.get())
				return _default;

			return current->data;
		}

		template <class Key, class Data>
		bool VTree<Key, Data>::exists(Key const &_key) const
		{
			cow_ptr<VNode<Key, Data>> current = findNode(_key);
			if (!current.get())
				return false;
			else
				return true;
		}

		template <class Key, class Data>
		size_t VTree<Key, Data>::mem_usage() const
		{
			size_t ret = sizeof(*this);
			if (!m_root) return ret;

			ret += m_root->mem_usage();
			return ret;
		}

		template <class Key, class Data>
		DArray<Data> *VTree<Key, Data>::ConvertToDArray() const
		{
			DArray<Data> *darray = new DArray<Data> ((int)m_size);
			RecursiveConvertToDArray(darray, m_root);
			return darray;
		}

		template <class Key, class Data>
		DArray<Key> *VTree<Key, Data>::ConvertIndexToDArray() const
		{
			DArray<Key> *darray = new DArray<Key> ((int)m_size);
			RecursiveConvertIndexToDArray(darray, m_root);
			return darray;
		}

		template <class Key, class Data>
		void VTree<Key, Data>::RecursiveConvertToDArray(DArray<Data> *darray, VNode<Key, Data> *btree) const
		{
			if (!btree) return;

			RecursiveConvertToDArray(darray, btree->left);
			darray->insert(btree->data);
			RecursiveConvertToDArray(darray, btree->right);
		}

		template <class Key, class Data>
		void VTree<Key, Data>::RecursiveConvertIndexToDArray(DArray<Key> *darray, VNode<Key, Data> *btree) const
		{
			if (!btree) return;

			RecursiveConvertIndexToDArray(darray, btree->left);
			darray->insert(btree->id);
			RecursiveConvertIndexToDArray(darray, btree->right);
		}
	}
}
