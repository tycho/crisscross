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

#ifndef __included_cc_ltree_h
#error "This file shouldn't be compiled directly."
#endif

namespace CrissCross
{
	namespace Data
	{
		template <class Key, class Data>
		LTree<Key, Data>::LTree()
		{
			m_size = 0;
		}

		template <class Key, class Data>
		LTree<Key, Data>::~LTree()
		{
			for (size_t i = 0; i < m_levels.size(); i++) {
				if (!m_levels.valid(i))
					break;
				delete m_levels.get(i);
				m_levels.remove(i);
			}
		}

		template <class Key, class Data>
		size_t LTree<Key, Data>::parent(size_t i)
		{
			if (i & 1 != 0)
				// Odd
				return (i - 1) / 2;
			else
				// Even
				return i / 2;
		}

		template <class Key, class Data>
		size_t LTree<Key, Data>::left(size_t i)
		{
			return i * 2;
		}

		template <class Key, class Data>
		size_t LTree<Key, Data>::right(size_t i)
		{
			return i * 2 + 1;
		}

		template <class Key, class Data>
		bool LTree<Key, Data>::erase(Key const &_key)
		{
			size_t level, index;
			bool found = findNode(_key, level, index);

			if (!found)
				return false;

			LNode<Key, Data> *node, *cleft = NULL, *cright = NULL;

			node = &m_levels[level][index];
			if (m_levels.valid(level + 1)) {
				cleft = &m_levels[level + 1][left(index)];
				cright = &m_levels[level + 1][right(index)];
			}

			--m_size;
			Dealloc(node->id);

			if (cleft && cright) {
				if (cleft->id != NullKey<Key>() &&
					cright->id != NullKey<Key>())
				{
					size_t min_l = level + 1, min_i = right(index);
					bool found = findMinimum(min_l, min_i);
					CoreAssert(found);
					LNode<Key, Data> *min = &m_levels[min_l][min_i];
					*node = *min;
				}
			}

			return true;
		}

		template <class Key, class Data>
		bool LTree<Key, Data>::replace(Key const &key, Data const &_data)
		{
			size_t level, index;
			bool found = findNode(key, &level, &index);
			if (!found)
				return false;
			LNode<Key, Data> &current = m_levels[level][index];
			current.data = _data;
			return true;
		}

		template <class Key, class Data>
		bool LTree<Key, Data>::insert(Key const &_key, Data const &_data)
		{
			size_t level = 0, index = 0;
			LNode<Key, Data> *dest = NULL;
			while (m_levels.valid(level)) {
				dest = &m_levels[level][index];
				if (dest->id == NullKey<Key>())
					break;
				int cmp = Compare(_key, dest->id);
				if (cmp < 0)
					index = left(index);
				else if (cmp > 0)
					index = right(index);
				else
					return false;
				level++;
			}

			if (!m_levels.valid(level)) {
				m_levels.insert(new LNode<Key, Data>[1 << level], level);
				dest = &m_levels[level][index];
			}

			dest->id = Duplicate(_key);
			dest->data = _data;

			++m_size;
			return true;
		}

		template <class Key, class Data>
		bool LTree<Key, Data>::findNode(Key const &_key, size_t &_level, size_t &_index) const
		{
			_level = _index = 0;
			while (m_levels.valid(_level)) {
				LNode<Key, Data> *p_current = &m_levels[_level][_index];
				if (p_current->id == NullKey<Key>())
					return false;
				int cmp = Compare(_key, p_current->id);
				if (cmp < 0)
					_index = left(_index);
				else if (cmp > 0)
					_index = right(_index);
				else {
					return true;
				}
				_level++;
			}
			return false;
		}

		template <class Key, class Data>
		bool LTree<Key, Data>::findMinimum(size_t &_level, size_t &_index) const
		{
			size_t l = _level, i = _index;
			if (!m_levels.valid(_level))
				return false;
			while (m_levels.valid(l)) {
				if (!m_levels.valid(_index))
					break;
				if (m_levels[l][i].id == NullKey<Key>())
					return true;
				_level = l;
				_index = i;
				i = left(i);
				l++;
			}
			return true;
		}

		template <class Key, class Data>
		Data LTree<Key, Data>::find(Key const &_key, Data const &_default) const
		{
			size_t level, index;
			bool found = findNode(_key, level, index);
			if (!found)
				return _default;
			return m_levels[level][index].data;
		}

		template <class Key, class Data>
		bool LTree<Key, Data>::exists(Key const &_key) const
		{
			size_t level, index;
			bool found = findNode(_key, level, index);
			return found;
		}

		template <class Key, class Data>
		size_t LTree<Key, Data>::mem_usage() const
		{
#if 0
			size_t ret = sizeof(*this);
			if (!m_root) return ret;

			ret += m_root->mem_usage();
			return ret;
#else
			return 0;
#endif
		}

		template <class Key, class Data>
		DArray<Data> *LTree<Key, Data>::ConvertToDArray() const
		{
#if 0
			DArray<Data> *darray = new DArray<Data> ((int)m_size);
			RecursiveConvertToDArray(darray, m_root);
			return darray;
#else
			return NULL;
#endif
		}

		template <class Key, class Data>
		DArray<Key> *LTree<Key, Data>::ConvertIndexToDArray() const
		{
#if 0
			DArray<Key> *darray = new DArray<Key> ((int)m_size);
			RecursiveConvertIndexToDArray(darray, m_root);
			return darray;
#else
			return NULL;
#endif
		}

		template <class Key, class Data>
		void LTree<Key, Data>::RecursiveConvertToDArray(DArray<Data> *darray, LNode<Key, Data> *btree) const
		{
#if 0
			if (!btree) return;

			RecursiveConvertToDArray(darray, btree->left);
			darray->insert(btree->data);
			RecursiveConvertToDArray(darray, btree->right);
#endif
		}

		template <class Key, class Data>
		void LTree<Key, Data>::RecursiveConvertIndexToDArray(DArray<Key> *darray, LNode<Key, Data> *btree) const
		{
#if 0
			if (!btree) return;

			RecursiveConvertIndexToDArray(darray, btree->left);
			darray->insert(btree->id);
			RecursiveConvertIndexToDArray(darray, btree->right);
#endif
		}
	}
}
