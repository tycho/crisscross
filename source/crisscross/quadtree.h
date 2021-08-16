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
		template <class T, class VectorType>
		struct QtNode
		{
		public:
			VectorType pos;
			float collisionRadius;
			T data;

			QtNode(T const &_data, VectorType const &_pos, float _collisionRadius) : collisionRadius(_collisionRadius), data(_data), pos(_pos)
			{ }
		};

		template <class VectorType>
		struct QtStats {
			VectorType pos_ll;
			VectorType pos_tr;
			uint32_t depth;
			size_t elements;
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
		          class VectorType,
		          int MaxDepth = 7,
		          int MaxNodesPerLevel = 32>
		class Quadtree final
		{
		protected:
			using nodetype_t = QtNode<T, VectorType>;

			VectorType llPosition;
			VectorType trPosition;
			int descentLevel;
			Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel> *ll;
			Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel> *lr;
			Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel> *tl;
			Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel> *tr;
			std::vector<nodetype_t> nodes;

			// Constructor for deeper levels
			Quadtree(VectorType const &lower_left, VectorType const &upper_right, int _descentLevel);

			static bool InRange(float lower_bound, float upper_bound, float point);
			static bool CircleCollision(VectorType circle1, float radius1, VectorType circle2, float radius2);

			void Descend();
			void Ascend();
		public:
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
			using predicate_t = QuadtreeSearchFunction<T>;
#endif
			using statstype_t = QtStats<VectorType>;

			Quadtree(VectorType const &lower_left, VectorType const &upper_right);
			~Quadtree();
			void Empty();
			bool IsEmpty(bool _childOnly);
			void InsertObject(T const &_object, VectorType const &position, float _collisionRadius);
			bool RemoveObject(T const &_object, VectorType const &position, float _collisionRadius);
			QuadtreeSearchResult ObjectsInCircle(std::vector<T> &array, VectorType const &circle, float radius, size_t maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
				, predicate_t pretest
				, predicate_t matchtest
#endif
			) const;

			void Collect(std::vector<T>& _elements, uint32_t &maxDepthSeen, uint32_t &maxNodesSeen, uint32_t currentDepth = 0);
			void CollectStats(std::vector<statstype_t> &rects, uint32_t currentDepth = 0);
		};

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::Collect(std::vector<T>& _elements, uint32_t &maxDepthSeen, uint32_t &maxNodesSeen, uint32_t currentDepth)
		{
			if (nodes.size() > maxNodesSeen)
				maxNodesSeen = nodes.size();
			if (currentDepth > maxDepthSeen)
				maxDepthSeen = currentDepth;
			for (auto& elem : nodes) {
				_elements.push_back(elem.data);
			}
			if (!ll)
				return;
			ll->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
			lr->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
			tl->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
			tr->Collect(_elements, maxDepthSeen, maxNodesSeen, currentDepth + 1);
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::CollectStats(std::vector<statstype_t> &_rects, uint32_t currentDepth)
		{
			statstype_t rect;
			rect.pos_ll = llPosition;
			rect.pos_tr = trPosition;
			rect.depth = currentDepth;
			rect.elements = nodes.size();
			_rects.push_back(rect);
			if (!ll)
				return;
			ll->CollectStats(_rects, currentDepth + 1);
			lr->CollectStats(_rects, currentDepth + 1);
			tl->CollectStats(_rects, currentDepth + 1);
			tr->CollectStats(_rects, currentDepth + 1);
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::InRange(float lower_bound, float upper_bound, float point)
		{
			return point > lower_bound && point <= upper_bound;
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::CircleCollision(VectorType circle1, float radius1, VectorType circle2, float radius2)
		{
			VectorType diff = circle1 - circle2;
			float maximumDistanceSquared = (radius1 + radius2) * (radius1 + radius2);
			float actualDistanceSquared = diff.x * diff.x + diff.y * diff.y;
			return actualDistanceSquared <= maximumDistanceSquared;
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		QuadtreeSearchResult Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::ObjectsInCircle(std::vector<T> &array, VectorType const &circle, float radius, size_t maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
			, predicate_t pretest
			, predicate_t matchtest
#endif
		) const
		{
			/* find objects stored in this quadtree */
			for (typename std::vector<nodetype_t>::const_iterator i = nodes.begin();
				 i != nodes.end();
				 i++)
			{
				nodetype_t const &node = *i;
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
				QuadtreeCallbackResponse response = pretest( node.data );
#else
				QuadtreeCallbackResponse response = QuadtreeCallbackResponse::ACCEPT;
#endif

				// Do early test for criteria match/mismatch, which should be cheaper than the circle collision test
				bool testCollision = response != QuadtreeCallbackResponse::REJECT;

				// If the early check didn't explicitly reject it, test for circle collision.
				if ( testCollision && CircleCollision( circle, radius, node.pos, node.collisionRadius ) )
				{
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					response = matchtest( node.data );

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
			float	x = circle.x,
					y = circle.y,
					left = x - radius,
					right = x + radius,
					top = y + radius,
					bottom = y - radius,
					midX = (llPosition.x + trPosition.x) * 0.5f,
					midY = (llPosition.y + trPosition.y) * 0.5f;

			if (top >= midY && left <= midX) {
				/* need to descend into top left quadtree */
				if ( tl->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, pretest, matchtest
#endif
				) == QuadtreeSearchResult::ABORTED )
				{
					return QuadtreeSearchResult::ABORTED;
				}
			}

			if (top >= midY && right >= midX) {
				/* top right quadtree */
				if ( tr->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, pretest, matchtest
#endif
				) == QuadtreeSearchResult::ABORTED )
				{
					return QuadtreeSearchResult::ABORTED;
				}
			}

			if (bottom <= midY && right >= midX) {
				/* lower right quadtree */
				if ( lr->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, pretest, matchtest
#endif
				) == QuadtreeSearchResult::ABORTED )
				{
					return QuadtreeSearchResult::ABORTED;
				}
			}

			if (bottom <= midY && left <= midX) {
				/* lower left quadtree */
				if ( ll->ObjectsInCircle( array, circle, radius, maxResults
#ifdef QUADTREE_SEARCH_CALLBACK_SUPPORTED
					, pretest, matchtest
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

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::Descend()
		{
			float	leftX = llPosition.x,
					rightX = trPosition.x,
					topY = trPosition.y,
					bottomY = llPosition.y,
					midX = (leftX + rightX) * 0.5f,
					midY = (topY + bottomY) * 0.5f;

			ll = new Quadtree(VectorType(leftX, bottomY), VectorType(midX, midY), descentLevel - 1);
			lr = new Quadtree(VectorType(midX, bottomY), VectorType(rightX, midY), descentLevel - 1);
			tl = new Quadtree(VectorType(leftX, midY), VectorType(midX, topY), descentLevel - 1);
			tr = new Quadtree(VectorType(midX, midY), VectorType(rightX, topY), descentLevel - 1);
			/* distribute all current nodes */
			std::vector<nodetype_t> oldCopy = std::move(nodes);
			nodes.clear();
			for (auto node : oldCopy)
				InsertObject(node.data, node.pos, node.collisionRadius);
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::Ascend()
		{
			if (!IsEmpty(true))
				return;
			delete ll; ll = NULL;
			delete lr; lr = NULL;
			delete tl; tl = NULL;
			delete tr; tr = NULL;
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::IsEmpty(bool _childOnly)
		{
			if (!_childOnly) {
				if (nodes.size() > 0)
					return false;
			}
			if (!ll)
				return true;
			if (!ll->IsEmpty(false)) return false;
			if (!lr->IsEmpty(false)) return false;
			if (!tl->IsEmpty(false)) return false;
			if (!tr->IsEmpty(false)) return false;
			return true;
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::Empty()
		{
			nodes.clear();
			if (!ll)
				return;
			ll->Empty();
			lr->Empty();
			tl->Empty();
			tr->Empty();
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		bool Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::RemoveObject(T const &_object, VectorType const &_position, float radius)
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
			float	x = _position.x,
					y = _position.y,
					left = x - radius,
					right = x + radius,
					top = y + radius,
					bottom = y - radius,
					midX = (llPosition.x + trPosition.x) * 0.5f,
					midY = (llPosition.y + trPosition.y) * 0.5f;

			if (bottom < midY && left < midX) {
				/* lower left quadtree */
				if ( ll->RemoveObject(_object, _position, radius) ) {
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
			return false;
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		void Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::InsertObject(T const &_object, VectorType const &_position, float radius)
		{
			if (descentLevel == 0 || (!ll && nodes.size() < MaxNodesPerLevel)) {
				nodes.push_back(nodetype_t(_object, _position, radius));
			} else {
				if (!ll)
					Descend();

				/* get relevant information for neatness checking */
				float	x = _position.x,
						y = _position.y,
						left = x - radius,
						right = x + radius,
						top = y + radius,
						bottom = y - radius,
						midX = (llPosition.x + trPosition.x) * 0.5f,
						midY = (llPosition.y + trPosition.y) * 0.5f;

				if (InRange(left, right, midX) ||
				    InRange(top, bottom, midY))	{
					/*
					 * Crosses the border between trees, so we insert at this
					 * level instead of a lower one.
					 */
					nodes.push_back(nodetype_t(_object, _position, radius));
				} else {
					/* Find target quadrant for insertion. */
					Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel> *target = nullptr;
					if (y <= midY)
					{
						if (x <= midX)
							target = ll;
						else
							target = lr;
					} else {
						if (x <= midX)
							target = tl;
						else
							target = tr;
					}
					CoreAssert(target != nullptr);
					target->InsertObject(_object, _position, radius);
				}
			}
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::Quadtree(VectorType const &lower_left, VectorType const &upper_right)
		:   llPosition(lower_left),
		    trPosition(upper_right),
		    ll(NULL),
		    lr(NULL),
		    tl(NULL),
		    tr(NULL),
		    descentLevel(MaxDepth)
		{
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::Quadtree(VectorType const &lower_left, VectorType const &upper_right, int _descentLevel)
		:   llPosition(lower_left),
		    trPosition(upper_right),
		    ll(NULL),
		    lr(NULL),
		    tl(NULL),
		    tr(NULL),
		    descentLevel(_descentLevel)
		{
		}

		template <class T, class VectorType, int MaxDepth, int MaxNodesPerLevel>
		Quadtree<T, VectorType, MaxDepth, MaxNodesPerLevel>::~Quadtree()
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
