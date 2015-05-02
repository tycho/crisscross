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

#ifndef __included_cc_quadtree_h
#define __included_cc_quadtree_h

#include <vector>

#include <crisscross/cc_attr.h>
#include <crisscross/compare.h>
#include <crisscross/mutex.h>
#include <crisscross/vec2.h>

namespace CrissCross
{
	namespace Data
	{
		template <class T>
		struct QtNode
		{
		public:
			float collisionRadius;
			T data;
			vec2 pos;

			QtNode(T const &_data, vec2 const &_pos, float _collisionRadius) : collisionRadius(_collisionRadius), data(_data), pos(_pos)
			{ }
		};

		template <class T,
		          int MaxDepth = 7,
		          int MaxNodesPerLevel = 32>
		class Quadtree
		{
		protected:
			vec2 llPosition;
			vec2 trPosition;
			Quadtree<T, MaxDepth, MaxNodesPerLevel>             * parent;
			Quadtree<T, MaxDepth, MaxNodesPerLevel>             * ll;
			Quadtree<T, MaxDepth, MaxNodesPerLevel>             * lr;
			Quadtree<T, MaxDepth, MaxNodesPerLevel>             * tl;
			Quadtree<T, MaxDepth, MaxNodesPerLevel>             * tr;
			int						descentLevel;
			std::vector<QtNode<T>*>	nodes;

			// Constructor for deeper levels
			Quadtree(vec2 const &lower_left, vec2 const &upper_right, int _descentLevel, Quadtree<T, MaxDepth, MaxNodesPerLevel> *_parent);

			static bool InRange(float lower_bound, float upper_bound, float point);
			static bool CircleCollision(vec2 circle1, float radius1, vec2 circle2, float radius2);

			void Descend();
			void Ascend();
		public:
			Quadtree(vec2 const &lower_left, vec2 const &upper_right);
			virtual ~Quadtree();
			virtual void InsertObject(T const &_object, vec2 const &position, float _collisionRadius);
			virtual bool RemoveObject(T const &_object, vec2 const &position, float _collisionRadius);
			virtual void ObjectsInCircle(std::vector<T> &array, vec2 const &circle, float radius, size_t limit = (size_t)-1) const;
		};

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, MaxDepth, MaxNodesPerLevel>::InRange(float lower_bound, float upper_bound, float point)
		{
			return point > lower_bound && point <= upper_bound;
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, MaxDepth, MaxNodesPerLevel>::CircleCollision(vec2 circle1, float radius1, vec2 circle2, float radius2)
		{
			float maximumDistanceSquared = (radius1 + radius2) * (radius1 + radius2);
			float actualDistanceSquared = vec2::DistanceSquared(circle1, circle2);
			return actualDistanceSquared <= maximumDistanceSquared;
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::ObjectsInCircle(std::vector<T> &array, vec2 const &circle, float radius, size_t limit) const
		{
			if (array.size() >= limit) return;

			/* find objects stored in this quadtree */
			for (typename std::vector<QtNode<T>*>::const_iterator i = nodes.begin();
				 i != nodes.end();
				 i++)
			{
				QtNode<T> *node = *i;
				if (CircleCollision(circle, radius, node->pos, node->collisionRadius)) {
					array.push_back(node->data);
					if (array.size() >= limit) return;
				}
			}

			if (!ll)  /* if no subtrees, return this as-is */
				return;

			/* find objects stored in the child quadtrees */
			float	x = circle.X(),
					y = circle.Y(),
					left = x - radius,
					right = x + radius,
					top = y + radius,
					bottom = y - radius,
					midX = (llPosition.X() + trPosition.X()) / 2.0f,
					midY = (llPosition.Y() + trPosition.Y()) / 2.0f;

			#ifdef _OPENMP
			std::vector<T> v1, v2, v3, v4;
			#pragma omp parallel sections
			{
				#pragma omp section
				if (top > midY && left < midX) {
					/* need to descend into top left quadtree */
					tl->ObjectsInCircle(v1, circle, radius);
				}

				#pragma omp section
				if (top > midY && right > midX)	{
					/* top right quadtree */
					tr->ObjectsInCircle(v2, circle, radius);
				}

				#pragma omp section
				if (bottom < midY && right > midX) {
					/* lower right quadtree */
					lr->ObjectsInCircle(v3, circle, radius);
				}

				#pragma omp section
				if (bottom < midY && left < midX) {
					/* lower left quadtree */
					ll->ObjectsInCircle(v4, circle, radius);
				}
			}
			/* Merge the resulting vectors. */
			array.insert(array.end(), v1.begin(), v1.end());
			array.insert(array.end(), v2.begin(), v2.end());
			array.insert(array.end(), v3.begin(), v3.end());
			array.insert(array.end(), v4.begin(), v4.end());
			#else
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
			#endif
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::Descend()
		{
			float	leftX = llPosition.X(),
					rightX = trPosition.X(),
					topY = trPosition.Y(),
					bottomY = llPosition.Y(),
					midX = (leftX + rightX) / 2.0f,
					midY = (topY + bottomY) / 2.0f;

			ll = new Quadtree(vec2(leftX, bottomY), vec2(midX, midY), descentLevel - 1, this);
			lr = new Quadtree(vec2(midX, bottomY), vec2(rightX, midY), descentLevel - 1, this);
			tl = new Quadtree(vec2(leftX, midY), vec2(rightX, midY), descentLevel - 1, this);
			tr = new Quadtree(vec2(midX, midY), vec2(rightX, topY), descentLevel - 1, this);
			/* distribute all current nodes */
			std::vector<QtNode<T>*> oldCopy = nodes;
			nodes.clear();
			for (typename std::vector<QtNode<T>*>::iterator i = oldCopy.begin();
				 i != oldCopy.end();
				 i++)
			{
				QtNode<T> *node = *i;
				InsertObject(node->data, node->pos, node->collisionRadius);
				delete node;
			}
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::Ascend()
		{
			if (ll)	{
				delete ll; ll = NULL;
				delete lr; lr = NULL;
				delete tl; tl = NULL;
				delete tr; tr = NULL;
			}
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, MaxDepth, MaxNodesPerLevel>::RemoveObject(T const &_object, vec2 const &_position, float radius)
		{
			/* find objects stored in this quadtree */
			for (typename std::vector<QtNode<T>*>::iterator i = nodes.begin();
				 i != nodes.end();
				 i++)
			{
				QtNode<T> *node = *i;
				if (CircleCollision(_position, radius, node->pos, node->collisionRadius)) {
					if (node->data == _object) {
						nodes.erase(i);
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
			float	x = _position.X(),
					y = _position.Y(),
					left = x - radius,
					right = x + radius,
					top = y + radius,
					bottom = y - radius,
					midX = (llPosition.X() + trPosition.X()) / 2.0f,
					midY = (llPosition.Y() + trPosition.Y()) / 2.0f;

			if (top > midY && left < midX) {
				/* need to descend into top left quadtree */
				if ( tl->RemoveObject(_object, _position, radius) )
					return true;
			}
			if (top > midY && right > midX)	{
				/* top right quadtree */
				if ( tr->RemoveObject(_object, _position, radius) )
					return true;
			}
			if (bottom < midY && right > midX) {
				/* lower right quadtree */
				if ( lr->RemoveObject(_object, _position, radius) )
					return true;
			}
			if (bottom < midY && left < midX) {
				/* lower left quadtree */
				if ( ll->RemoveObject(_object, _position, radius) )
					return true;
			}
			return false;
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::InsertObject(T const &_object, vec2 const &_position, float radius)
		{
			if (nodes.size() < MaxNodesPerLevel || descentLevel == 0) {
				nodes.push_back(new QtNode<T>(_object, _position, radius));
			} else {
				if (!ll)
					Descend();

				/* get relevant information for neatness checking */
				float	x = _position.X(),
						y = _position.Y(),
						left = x - radius,
						right = x + radius,
						top = y + radius,
						bottom = y - radius,
						midX = (llPosition.X() + trPosition.X()) / 2.0f,
						midY = (llPosition.Y() + trPosition.Y()) / 2.0f;

				/* is it a nasty case, crossing the borderline? */
				if (InRange(left, right, midX) ||
				    InRange(top, bottom, midY))	{
					/* yes, it is :/ */
					nodes.push_back(new QtNode<T>(_object, _position, radius));
				} else {
					if (!ll)  /* create the subquadrants */
						Descend();
					/* determine the quadrant */
					if (x < midX && y < midY) {
						ll->InsertObject(_object, _position, radius);
					} else if (x > midX && y < midY) {
						lr->InsertObject(_object, _position, radius);
					} else if (x < midX && y > midY) {
						tl->InsertObject(_object, _position, radius);
					} else {
						tr->InsertObject(_object, _position, radius);
					}
				}
			}
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		Quadtree<T, MaxDepth, MaxNodesPerLevel>::Quadtree(vec2 const &lower_left, vec2 const &upper_right)
		:   llPosition(lower_left),
		    trPosition(upper_right),
		    parent(NULL),
		    ll(NULL),
		    lr(NULL),
		    tl(NULL),
		    tr(NULL),
		    descentLevel(MaxDepth)
		{
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		Quadtree<T, MaxDepth, MaxNodesPerLevel>::Quadtree(vec2 const &lower_left, vec2 const &upper_right, int _descentLevel, Quadtree<T, MaxDepth, MaxNodesPerLevel> *_parent)
		:   llPosition(lower_left),
		    trPosition(upper_right),
		    parent(_parent),
		    ll(NULL),
		    lr(NULL),
		    tl(NULL),
		    tr(NULL),
		    descentLevel(_descentLevel)
		{
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		Quadtree<T, MaxDepth, MaxNodesPerLevel>::~Quadtree()
		{
			Ascend();

			for (typename std::vector<QtNode<T>*>::iterator i = nodes.begin();
				 i != nodes.end();
				 i++)
			{
				delete *i;
			}
			nodes.clear();
		}
	}
}

#endif
