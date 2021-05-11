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

#ifndef __included_cc_quadtree_h
#define __included_cc_quadtree_h

#include <vector>
#include <functional>

#include <crisscross/cc_attr.h>
#include <crisscross/compare.h>
#include <crisscross/mutex.h>
#include <crisscross/vec2.h>

#if __cplusplus >= 202002L || __cpp_generic_lambdas >= 201707
#define QUADTREE_SEARCH_CALLBACK_SUPPORTED
#endif

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

		enum class QuadtreeCallbackResponse
		{
			ACCEPT,
			REJECT,
			ACCEPT_AND_STOP,
			REJECT_AND_STOP,
			STOP_ITERATION
		};

		enum class QuadtreeSearchResult
		{
			SUCCESS,
			NOT_FOUND,
			ABORTED
		};

#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
		template<class T>
		using QuadtreeSearchFunction = std::function<QuadtreeCallbackResponse ( T const & )>;
#endif

		template <class T,
		          int MaxDepth = 7,
		          int MaxNodesPerLevel = 32>
		class Quadtree final
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
			std::vector<QtNode<T>>	nodes;

			// Constructor for deeper levels
			Quadtree(vec2 const &lower_left, vec2 const &upper_right, int _descentLevel, Quadtree<T, MaxDepth, MaxNodesPerLevel> *_parent);

			static bool InRange(float lower_bound, float upper_bound, float point);
			static bool CircleCollision(vec2 circle1, float radius1, vec2 circle2, float radius2);

			void Descend();
			void Ascend();
		public:
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
			using predicate_t = QuadtreeSearchFunction<T>;
#endif

			Quadtree(vec2 const &lower_left, vec2 const &upper_right);
			~Quadtree();
			void Empty();
			void InsertObject(T const &_object, vec2 const &position, float _collisionRadius);
			bool RemoveObject(T const &_object, vec2 const &position, float _collisionRadius);
			QuadtreeSearchResult ObjectsInCircle(std::vector<T> &array, vec2 const &circle, float radius, size_t maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
				, predicate_t predicate
#endif
			) const;

			void Collect(std::vector<T>& _elements, uint32_t &maxDepthSeen, uint32_t &maxNodesSeen, uint32_t currentDepth = 0);
		};

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::Collect(std::vector<T>& _elements, uint32_t &maxDepthSeen, uint32_t &maxNodesSeen, uint32_t currentDepth)
		{
			if (nodes.size() > maxNodesSeen)
				maxNodesSeen = nodes.size();
			if (currentDepth > maxDepthSeen)
				maxDepthSeen = currentDepth;
			for (auto& elem : nodes) {
				_elements.push_back(elem.data);
			}
			if (ll) ll->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
			if (lr) lr->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
			if (tl) tl->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
			if (tr) tr->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
		}

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
		QuadtreeSearchResult Quadtree<T, MaxDepth, MaxNodesPerLevel>::ObjectsInCircle(std::vector<T> &array, vec2 const &circle, float radius, size_t maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
			, predicate_t predicate
#endif
		) const
		{
			/* find objects stored in this quadtree */
			for (typename std::vector<QtNode<T>>::const_iterator i = nodes.begin();
				 i != nodes.end();
				 i++)
			{
				QtNode<T> const &node = *i;
				if ( CircleCollision( circle, radius, node.pos, node.collisionRadius ) )
				{
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					QuadtreeCallbackResponse response = predicate( node.data );
					if ( response == QuadtreeCallbackResponse::ACCEPT ||
						 response == QuadtreeCallbackResponse::ACCEPT_AND_STOP )
					{
						array.push_back( node.data );
					}
					if ( response == QuadtreeCallbackResponse::STOP_ITERATION ||
						 response == QuadtreeCallbackResponse::ACCEPT_AND_STOP ||
						 response == QuadtreeCallbackResponse::REJECT_AND_STOP )
					{
						return QuadtreeSearchResult::ABORTED;
					}
#else
					array.push_back( node.data );
					if ( array.size() >= maxResults )
						return QuadtreeSearchResult::ABORTED;
#endif
				}
			}

			if ( !ll )  /* if no subtrees, return this as-is */
			{
				if ( array.size() > 0 )
					return QuadtreeSearchResult::SUCCESS;
				else
					return QuadtreeSearchResult::NOT_FOUND;
			}

			/* find objects stored in the child quadtrees */
			float	x = circle.X(),
					y = circle.Y(),
					left = x - radius,
					right = x + radius,
					top = y + radius,
					bottom = y - radius,
					midX = (llPosition.X() + trPosition.X()) / 2.0f,
					midY = (llPosition.Y() + trPosition.Y()) / 2.0f;

			if (top > midY && left < midX) {
				/* need to descend into top left quadtree */
				if ( tl->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, predicate
#endif
				) == QuadtreeSearchResult::ABORTED )
				{
					return QuadtreeSearchResult::ABORTED;
				}
			}

			if (top > midY && right > midX)	{
				/* top right quadtree */
				if ( tr->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, predicate
#endif
				) == QuadtreeSearchResult::ABORTED )
				{
					return QuadtreeSearchResult::ABORTED;
				}
			}

			if (bottom < midY && right > midX) {
				/* lower right quadtree */
				if ( lr->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, predicate
#endif
				) == QuadtreeSearchResult::ABORTED )
				{
					return QuadtreeSearchResult::ABORTED;
				}
			}

			if (bottom < midY && left < midX) {
				/* lower left quadtree */
				if ( ll->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, predicate
#endif
				) == QuadtreeSearchResult::ABORTED )
				{
					return QuadtreeSearchResult::ABORTED;
				}
			}

			if ( array.size() > 0 )
				return QuadtreeSearchResult::SUCCESS;
			else
				return QuadtreeSearchResult::NOT_FOUND;
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
			tl = new Quadtree(vec2(leftX, midY), vec2(midX, topY), descentLevel - 1, this);
			tr = new Quadtree(vec2(midX, midY), vec2(rightX, topY), descentLevel - 1, this);
			/* distribute all current nodes */
			std::vector<QtNode<T>> oldCopy = nodes;
			nodes.clear();
			for (auto node : oldCopy)
				InsertObject(node.data, node.pos, node.collisionRadius);
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::Ascend()
		{
			if (!ll)
				return;
			if (ll->nodes.size() != 0 ||
			    lr->nodes.size() != 0 ||
				tl->nodes.size() != 0 ||
				tr->nodes.size() != 0)
				return;
			delete ll; ll = NULL;
			delete lr; lr = NULL;
			delete tl; tl = NULL;
			delete tr; tr = NULL;
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::Empty()
		{
			nodes.clear();
			if (ll) ll->Empty();
			if (lr) lr->Empty();
			if (tl) tl->Empty();
			if (tr) tr->Empty();
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, MaxDepth, MaxNodesPerLevel>::RemoveObject(T const &_object, vec2 const &_position, float radius)
		{
			/* find objects stored in this quadtree */
			for (auto node = nodes.begin(); node != nodes.end(); node++)
			{
				if (CircleCollision(_position, radius, node->pos, node->collisionRadius)) {
					if (node->data == _object) {
						std::swap(*node, nodes.back());
						nodes.pop_back();
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
				if ( tl->RemoveObject(_object, _position, radius) ) {
					Ascend();
					return true;
				}
			}
			if (top > midY && right > midX)	{
				/* top right quadtree */
				if ( tr->RemoveObject(_object, _position, radius) ) {
					Ascend();
					return true;
				}
			}
			if (bottom < midY && right > midX) {
				/* lower right quadtree */
				if ( lr->RemoveObject(_object, _position, radius) ) {
					Ascend();
					return true;
				}
			}
			if (bottom < midY && left < midX) {
				/* lower left quadtree */
				if ( ll->RemoveObject(_object, _position, radius) ) {
					Ascend();
					return true;
				}
			}
			return false;
		}

		template <class T, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, MaxDepth, MaxNodesPerLevel>::InsertObject(T const &_object, vec2 const &_position, float radius)
		{
			if (nodes.size() < MaxNodesPerLevel || descentLevel == 0) {
				nodes.push_back(QtNode<T>(_object, _position, radius));
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
					nodes.push_back(QtNode<T>(_object, _position, radius));
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
			delete ll; ll = NULL;
			delete lr; lr = NULL;
			delete tl; tl = NULL;
			delete tr; tr = NULL;
			nodes.clear();
		}
	}
}

#endif
