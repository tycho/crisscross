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

#include <vector>

#include <crisscross/compare.h>
#include <crisscross/vec2.h>

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
				Quadtree<T> * parent;
				Quadtree<T> * ll;
				Quadtree<T> * lr;
				Quadtree<T> * tl;
				Quadtree<T> * tr;
				int descentLevel;
				std::vector<QtNode<T> *> nodes;

				static bool InRange(float lower_bound, float upper_bound, float point);
				static bool CircleCollision(vec2 circle1, float radius1, vec2 circle2, float radius2);

				void Descend();
				void Ascend();
			public:
				Quadtree(vec2 const &lower_left, vec2 const &upper_right, int _descentLevel = 7, Quadtree * _parent = NULL);
				~Quadtree();
				void InsertObject(T const &_object, vec2 const &position, float _collisionRadius);
				bool RemoveObject(T const &_object, vec2 const &position, float _collisionRadius);
				std::vector<T> ObjectsInCircle(vec2 const &_centre, float radius);
		};

		template <class T>
		bool Quadtree<T>::InRange(float lower_bound, float upper_bound, float point)
		{
			return point > lower_bound && point <= upper_bound;
		}

		template <class T>
		bool Quadtree<T>::CircleCollision(vec2 circle1, float radius1, vec2 circle2, float radius2)
		{
			float maximumDistanceSquared = (radius1 + radius2) * (radius1 + radius2);
			float actualDistanceSquared = vec2::DistanceSquared(circle1, circle2);
			return actualDistanceSquared <= maximumDistanceSquared;
		}

		template <class T>
		std::vector<T> Quadtree<T>::ObjectsInCircle(vec2 const &circle, float radius)
		{
			std::vector<T> returnValue;
			typename std::vector<QtNode<T> *>::iterator iter;
			/* find objects stored in this quadtree */
			for (iter = nodes.begin(); iter != nodes.end(); iter++)	{
				if (CircleCollision(circle, radius, (*iter)->pos, (*iter)->collisionRadius)) {
					returnValue.push_back((*iter)->data);
				}
			}
			if (!ll)  /* if no subtrees, return this as-is */
				return returnValue;
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
				std::vector<T> resultingVector = tl->ObjectsInCircle(circle, radius);
				returnValue.insert(returnValue.end(), resultingVector.begin(), resultingVector.end());
			}
			if (top > midY && right > midX)	{
				/* top right quadtree */
				std::vector<T> resultingVector = tr->ObjectsInCircle(circle, radius);
				returnValue.insert(returnValue.end(), resultingVector.begin(), resultingVector.end());
			}
			if (bottom < midY && right > midX) {
				/* lower right quadtree */
				std::vector<T> resultingVector = lr->ObjectsInCircle(circle, radius);
				returnValue.insert(returnValue.end(), resultingVector.begin(), resultingVector.end());
			}
			if (bottom < midY && left < midX) {
				/* lower left quadtree */
				std::vector<T> resultingVector = ll->ObjectsInCircle(circle, radius);
				returnValue.insert(returnValue.end(), resultingVector.begin(), resultingVector.end());
			}
			return returnValue;
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
			std::vector<QtNode<T> *> oldCopy = nodes;
			typename std::vector<QtNode<T> *>::iterator iter;
			nodes.clear();
			for (iter = oldCopy.begin(); iter != oldCopy.end(); iter++) {
				InsertObject((*iter)->data, (*iter)->pos, (*iter)->collisionRadius);
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
			typename std::vector<QtNode<T> *>::iterator iter;
			/* find objects stored in this quadtree */
			for (iter = nodes.begin(); iter != nodes.end();) {
				if (CircleCollision(_position, radius, (*iter)->pos, (*iter)->collisionRadius))	{
					if ((*iter)->data == _object) {
						QtNode<T> *node = (*iter);
						iter = nodes.erase(iter);
						delete node;
						/* check for ascension */
						if (parent) {
							if (parent->ll->nodes.empty() &&
							    parent->lr->nodes.empty() &&
							    parent->tr->nodes.empty() &&
							    parent->tl->nodes.empty()) {
								parent->Ascend();
							}
						}
						return true;
					}
				}
				iter++;
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
				nodes.push_back(new QtNode<T>(_object, _position, _collisionRadius));
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
					nodes.push_back(new QtNode<T>(_object, _position, _collisionRadius));
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

			typename std::vector<QtNode<T> *>::iterator iter;
			for (iter = nodes.begin(); iter != nodes.end();) {
				QtNode<T> *node = *iter;
				delete node;
				iter = nodes.erase(iter);
			}
		}
	}
}

#endif
