/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_quadtree_h
#define __included_cc_quadtree_h

#include <crisscross/compare.h>
#include <crisscross/mutex.h>
#include <crisscross/vec2.h>
#include <crisscross/darray.h>

namespace CrissCross
{
	namespace Data
	{
		const unsigned qtMax = 7;

		template <class T>
		struct QtNode
		{
		public:
			float collisionRadius;
			T data;
			vec2 pos;

			QtNode(T const &_data, vec2 const &_pos, float _collisionRadius) : collisionRadius(_collisionRadius), data(_data), pos(_pos)
			{
			};
		};

		template <class T>
		class Quadtree
		{
		protected:
			vec2 llPosition;
			vec2 trPosition;
			Quadtree<T>             * parent;
			Quadtree<T>             * ll;
			Quadtree<T>             * lr;
			Quadtree<T>             * tl;
			Quadtree<T>             * tr;
			int descentLevel;
			DArray<QtNode<T> *> nodes;

			static bool InRange(float lower_bound, float upper_bound, float point);
			static bool CircleCollision(vec2 circle1, float radius1, vec2 circle2, float radius2);

			void Descend();
			void Ascend();
		public:
			Quadtree(vec2 const &lower_left, vec2 const &upper_right, int _descentLevel = 7, Quadtree<T> * _parent = NULL);
			~Quadtree();
			void InsertObject(T const &_object, vec2 const &position, float _collisionRadius);
			bool RemoveObject(T const &_object, vec2 const &position, float _collisionRadius);
			void ObjectsInCircle(DArray<T> &array, vec2 const &circle, float radius);
		};

		template <class T>
		class ThreadSafeQuadtree : public Quadtree<T>
		{
		protected:
			CrissCross::System::ReadWriteLock m_lock;
		public:
			ThreadSafeQuadtree(vec2 const &lower_left, vec2 const &upper_right, int _descentLevel = 7, Quadtree<T> * _parent = NULL)
				: Quadtree<T>(lower_left, upper_right, _descentLevel, _parent)
			{};
			inline DArray<T> ObjectsInCircle(vec2 const &circle, float radius) {
				CrissCross::System::RWLockHolder rwlh(&m_lock, CrissCross::System::RW_LOCK_READ);
				return Quadtree<T>::ObjectsInCircle(circle, radius);
			};
			inline void Descend() {
				CrissCross::System::RWLockHolder rwlh(&m_lock, CrissCross::System::RW_LOCK_WRITE);
				Quadtree<T>::Descend();
			};
			inline void Ascend() {
				CrissCross::System::RWLockHolder rwlh(&m_lock, CrissCross::System::RW_LOCK_WRITE);
				Quadtree<T>::Ascend();
			};
			inline bool RemoveObject(T const &_object, vec2 const &_position, float radius) {
				CrissCross::System::RWLockHolder rwlh(&m_lock, CrissCross::System::RW_LOCK_WRITE);
				return Quadtree<T>::RemoveObject(_object, _position, radius);
			};
			inline void InsertObject(T const &_object, vec2 const &_position, float _collisionRadius) {
				CrissCross::System::RWLockHolder rwlh(&m_lock, CrissCross::System::RW_LOCK_WRITE);
				Quadtree<T>::InsertObject(_object, _position, _collisionRadius);
			};
		};

		template <class T>
		bool Quadtree<T>::InRange(float lower_bound, float upper_bound, float point)
		{
			return point > lower_bound && point <= upper_bound;
		}

		template <class T>
		inline bool Quadtree<T>::CircleCollision(vec2 circle1, float radius1, vec2 circle2, float radius2)
		{
			float maximumDistanceSquared = (radius1 + radius2) * (radius1 + radius2);
			float actualDistanceSquared = vec2::DistanceSquared(circle1, circle2);
			return actualDistanceSquared <= maximumDistanceSquared;
		}

		template <class T>
		void Quadtree<T>::ObjectsInCircle(DArray<T> &array, vec2 const &circle, float radius)
		{
			array.empty();

			/* find objects stored in this quadtree */
			for (unsigned i = 0; i < nodes.size(); i++) {
				if (nodes.valid(i) && CircleCollision(circle, radius, nodes[i]->pos, nodes[i]->collisionRadius)) {
					array.insert(nodes[i]->data);
				}
			}

			if (!ll)  /* if no subtrees, return this as-is */
				return;

			/* find objects stored in the child quadtrees */
			float x, y;
			float left, right;
			float top, bottom;
			float midX, midY;
			x = circle.X();
			y = circle.Y();
			left = x - radius;
			right = x + radius;
			top = y + radius;
			bottom = y - radius;
			midX = (llPosition.X() + trPosition.X()) / 2.0f;
			midY = (llPosition.Y() + trPosition.Y()) / 2.0f;
			if (top > midY && left < midX) {
				/* need to descend into top left quadtree */
				tl->ObjectsInCircle(array, circle, radius);
			}
			if (top > midY && right > midX)	{
				/* top right quadtree */
				tr->ObjectsInCircle(array, circle, radius);
			}
			if (bottom < midY && right > midX) {
				/* lower right quadtree */
				lr->ObjectsInCircle(array, circle, radius);
			}
			if (bottom < midY && left < midX) {
				/* lower left quadtree */
				ll->ObjectsInCircle(array, circle, radius);
			}
		}

		template <class T>
		void Quadtree<T>::Descend()
		{
			float leftX, rightX, midX, topY, bottomY, midY;
			leftX = llPosition.X();
			rightX = trPosition.X();
			topY = trPosition.Y();
			bottomY = llPosition.Y();
			midX = (leftX + rightX) / 2.0f;
			midY = (topY + bottomY) / 2.0f;
			ll = new Quadtree(vec2(leftX, bottomY), vec2(midX, midY), descentLevel - 1, this);
			lr = new Quadtree(vec2(midX, bottomY), vec2(rightX, midY), descentLevel - 1, this);
			tl = new Quadtree(vec2(leftX, midY), vec2(rightX, midY), descentLevel - 1, this);
			tr = new Quadtree(vec2(midX, midY), vec2(rightX, topY), descentLevel - 1, this);
			/* distribute all current nodes */
			DArray<QtNode<T> *> oldCopy = nodes;
			nodes.setSize(0);
			for (unsigned i = 0; i < oldCopy.size(); i++) {
				if (oldCopy.valid(i)) {
					InsertObject(oldCopy[i]->data, oldCopy[i]->pos, oldCopy[i]->collisionRadius);
				}
			}
		}

		template <class T>
		void Quadtree<T>::Ascend()
		{
			if (ll)	{
				delete ll; ll = NULL;
				delete lr; lr = NULL;
				delete tl; tl = NULL;
				delete tr; tr = NULL;
			}
		}

		template <class T>
		bool Quadtree<T>::RemoveObject(T const &_object, vec2 const &_position, float radius)
		{
			/* find objects stored in this quadtree */
			for (unsigned i = 0; i < nodes.size(); i++) {
				if (nodes.valid(i) && CircleCollision(_position, radius, nodes[i]->pos, nodes[i]->collisionRadius))	{
					if (nodes[i]->data == _object) {
						QtNode<T> *node = nodes[i];
						nodes.remove(i);
						delete node;
						/* check for ascension */
						if (parent) {
							if (parent->ll->nodes.size() == 0 &&
							    parent->lr->nodes.size() == 0 &&
							    parent->tr->nodes.size() == 0 &&
							    parent->tl->nodes.size() == 0) {
								parent->Ascend();
							}
						}
						return true;
					}
				}
			}
			if (!ll)  /* if no subtrees, return this as-is */
				return false;
			/* find objects stored in the child quadtrees */
			float x, y;
			float left, right;
			float top, bottom;
			float midX, midY;
			x = _position.X();
			y = _position.Y();
			left = x - radius;
			right = x + radius;
			top = y + radius;
			bottom = y - radius;
			midX = (llPosition.X() + trPosition.X()) / 2.0f;
			midY = (llPosition.Y() + trPosition.Y()) / 2.0f;
			if (top > midY && left < midX) {
				/* need to descend into top left quadtree */
				if (tl->RemoveObject(_object, _position, radius))
					return true;
			}
			if (top > midY && right > midX)	{
				/* top right quadtree */
				if (tr->RemoveObject(_object, _position, radius))
					return true;
			}
			if (bottom < midY && right > midX) {
				/* lower right quadtree */
				if (lr->RemoveObject(_object, _position, radius))
					return true;
			}
			if (bottom < midY && left < midX) {
				/* lower left quadtree */
				if (ll->RemoveObject(_object, _position, radius))
					return true;
			}
			return false;
		}

		template <class T>
		void Quadtree<T>::InsertObject(T const &_object, vec2 const &_position, float _collisionRadius)
		{
			if (nodes.size() < qtMax || descentLevel == 0) {
				nodes.insert(new QtNode<T>(_object, _position, _collisionRadius));
			} else {
				if (!ll)
					Descend();
				/* get relevant information for neatness checking */
				float radius;
				float x, y;
				float left, right;
				float top, bottom;
				float midX, midY;
				x = _position.X();
				y = _position.Y();
				radius = _collisionRadius;
				left = x - radius;
				right = x + radius;
				top = y + radius;
				bottom = y - radius;
				midX = (llPosition.X() + trPosition.X()) / 2.0f;
				midY = (llPosition.Y() + trPosition.Y()) / 2.0f;

				/* is it a nasty case, crossing the borderline? */
				if (InRange(left, right, midX) ||
				    InRange(top, bottom, midY))	{
					/* yes, it is :/ */
					nodes.insert(new QtNode<T>(_object, _position, _collisionRadius));
				} else {
					if (!ll)  /* create the subquadrants */
						Descend();
					/* determine the quadrant */
					if (x < midX && y < midY) {
						ll->InsertObject(_object, _position, _collisionRadius);
					} else if (x > midX && y < midY) {
						lr->InsertObject(_object, _position, _collisionRadius);
					} else if (x < midX && y > midY) {
						tl->InsertObject(_object, _position, _collisionRadius);
					} else {
						tr->InsertObject(_object, _position, _collisionRadius);
					}
				}
			}
		}

		template <class T>
		Quadtree<T>::Quadtree(vec2 const &lower_left, vec2 const &upper_right, int _descentLevel, Quadtree<T> * _parent) : llPosition(lower_left),
			trPosition(upper_right),
			parent(_parent),
			ll(NULL),
			lr(NULL),
			tl(NULL),
			tr(NULL),
			descentLevel(_descentLevel)
		{
		}

		template <class T>
		Quadtree<T>::~Quadtree()
		{
			Ascend();

			for (unsigned i = 0; i < nodes.size(); i++) {
				if (nodes.valid(i)) {
					delete nodes[i];
				}
			}
			nodes.empty();
		}
	}
}

#endif
