#include "RoutePlanner.h"
#include <functional>
#include <queue>
#include <unordered_map>

Route::Route() :
	nodeid_sequence(),
	sum_weight(numeric_limits<float>::max())
{
}

Route::Route(vector<unsigned int> nodeid_sequence,
			 float                cost) :
	nodeid_sequence(std::move(nodeid_sequence)),
	sum_weight(cost)
{
}

Route RoutePlanner::plan_route(unsigned int s,
							   unsigned int t,
							   const Graph& G,
							   Algorithm    algorithm)
{
	switch (algorithm)
	{
		case Dijkstra:
		{
			unordered_map<unsigned int, float>        dist;   // dist[u] 'will be' the distance of the shortest s->u path
			unordered_map<unsigned int, unsigned int> prev;   // prev[x] 'will be' the node before x in the shortest s->u path, via x
			using ui_f = pair<unsigned int, float>;
			priority_queue<ui_f,
						   vector<ui_f>,
						   function<bool(const ui_f&, const ui_f&)>>
					pq([&](const ui_f& p1, const ui_f& p2) {
						return p1.second > p2.second;
					});

			dist[s] = 0;                // set distance of s->s path to 0
			pq.push(make_pair(s, 0));   // add s to pq

			// While our priority queue is not empty
			while (!pq.empty())
			{
				// Nodes can be added more than once in the priority queue.
				// We need to remove them by checking if they are up-to-date.
				// This is common for Dijkstra implementations.
				while (pq.top().second > dist[pq.top().first])
				{
					pq.pop();
				}
				if (pq.empty())
				{
					break;
				}

				const unsigned int u = pq.top().first;
				pq.pop();

				// When t is popped, the shortest path s->t is computed.
				if (pq.top().first == t)
					break;

				for (const auto& e : G.GetAdjacentEdges(u))
				{
					const float len           = dist[u] + e.w;
					const auto  find_dist_e_v = dist.find(e.v);
					// If dist[v] is Infinite, or dist[u] + e.w < dist[e.v]
					if (find_dist_e_v == dist.end() || len < find_dist_e_v->second)
					{
						dist[e.v] = len;
						prev[e.v] = u;
						pq.push(make_pair(e.v, len));   // The same node can be added twice. We need to check if the value is up-to-date
					}
				}
			}

			// If there is no s->t path
			const auto& find_t = dist.find(t);
			if (find_t == dist.end())
			{
				return Route();
			}

			// Calculate the path vector
			vector<unsigned int> shortest_path;
			unsigned int         p = t;
			do
			{
				shortest_path.push_back(p);
				p = prev[p];
			} while (p != s);
			shortest_path.push_back(s);                            // add first node
			reverse(shortest_path.begin(), shortest_path.end());   // reverse the vector

			return Route(shortest_path, dist[t]);
		}
		case Bidirectional_Dijkstra:
		{
			// Forward Dijkstra
			unordered_map<unsigned int, float>        dist;   // dist[u] 'will be' the distance of the shortest s->u path
			unordered_map<unsigned int, unsigned int> prev;   // prev[x] 'will be' the node before x in the shortest s->u path, via x
			using ui_f = pair<unsigned int, float>;
			priority_queue<ui_f,
						   vector<ui_f>,
						   function<bool(const ui_f&, const ui_f&)>>
					pq([&](const ui_f& p1, const ui_f& p2) {
						return p1.second > p2.second;
					});

			dist[s] = 0;                // set distance of s->s path to 0
			pq.push(make_pair(s, 0));   // add s to pq

			// Backward Dijkstra
			unordered_map<unsigned int, float>        dist2;   // dist2[u] 'will be' the distance of the shortest u<-t path
			unordered_map<unsigned int, unsigned int> prev2;   // prev2[x] 'will be' the node before x in the shortest u<-t path, via x
			using ui_f = pair<unsigned int, float>;
			priority_queue<ui_f,
						   vector<ui_f>,
						   function<bool(const ui_f&, const ui_f&)>>
					pq2([&](const ui_f& p1, const ui_f& p2) {
						return p1.second > p2.second;
					});

			dist2[t] = 0;                // set distance of t->t path to 0
			pq2.push(make_pair(t, 0));   // add t to pq2

			unsigned int common_node = -1;
			float        best_dist   = numeric_limits<float>::max();
			// While our priority queue is not empty
			while (!pq.empty() &&
				   !pq2.empty() &&
				   dist[pq.top().first] + dist2[pq2.top().first] < best_dist)
			{
				// Forward Dijkstra
				while (pq.top().second > dist[pq.top().first])
				{
					pq.pop();   // delete if node was added more than once (value not up-to-date)
				}
				if (pq.empty())
				{
					break;
				}

				unsigned int u = pq.top().first;
				pq.pop();

				for (const auto& e : G.GetAdjacentEdges(u))
				{
					const float len           = dist[u] + e.w;
					const auto  find_dist_e_v = dist.find(e.v);
					// If dist[e.v] is Infinite, or dist[u] + e.w < dist[e.v]
					if (find_dist_e_v == dist.end() || len < find_dist_e_v->second)
					{
						dist[e.v] = len;
						prev[e.v] = u;
						pq.push(make_pair(e.v, len));
					}

					// If node e.v has been visited by backward dijkstra,
					// and it is a "better" common node, then update accordingly
					const auto find_dist2_e_v = dist2.find(e.v);
					if (find_dist2_e_v != dist2.end())
					{
						float d = dist[e.v] + find_dist2_e_v->second;   // dist of { s--> e.v --> t }
						if (d < best_dist)
						{
							common_node = e.v;   // update common_node
							best_dist   = d;     // update best_dist
						}
					}
				}

				// Backward Dijkstra
				while (pq2.top().second > dist2[pq2.top().first])
				{
					pq2.pop();   // delete if node was added more than once (value not up-to-date)
				}
				if (pq2.empty())
				{
					break;
				}

				u = pq2.top().first;
				pq2.pop();

				for (const auto& e : G.GetAdjacentEdges(u, true))
				{
					const float len            = dist2[u] + e.w;
					const auto  find_dist2_e_v = dist2.find(e.v);
					// If dist2[e.v] is Infinite, or dist2[u] + e.w < dist2[e.v]
					if (find_dist2_e_v == dist2.end() || len < find_dist2_e_v->second)
					{
						dist2[e.v] = len;
						prev2[e.v] = u;
						pq2.push(make_pair(e.v, len));
					}

					// If node e.v has been visited by forward dijkstra,
					// and it is a "better" common node, then update accordingly
					const auto find_dist_e_v = dist.find(e.v);
					if (find_dist_e_v != dist.end())
					{
						float d = dist2[e.v] + find_dist_e_v->second;   // dist of { t --> e.v --> s }
						if (d < best_dist)
						{
							common_node = e.v;   // update common_node
							best_dist   = d;     // update best_dist
						}
					}
				}
			}

			// If there is no s->t path
			if (common_node == -1)
			{
				return Route();
			}

			vector<unsigned int> shortest_path;

			// Calculate the path vector by combining the shortest path from s -> c and c <- t
			if (common_node != s)
			{
				unsigned int p = common_node;

				do
				{
					p = prev[p];
					shortest_path.push_back(p);
				} while (p != s);
				reverse(shortest_path.begin(), shortest_path.end());   // reverse the vector
			}
			shortest_path.push_back(common_node);
			if (common_node != t)
			{
				unsigned int p = common_node;

				do
				{
					p = prev2[p];
					shortest_path.push_back(p);
				} while (p != t);
			}

			return Route(shortest_path, best_dist);
		}
	}
}

// void ShortestPath::BidirectionalDijkstra(const QueryGraph &queryG, vector<int> &shortest_path, float &path_cost)
//{
//     using _pair = pair<int, float>;
//
//     // ====== Forward Dijkstra ======
//     unordered_map<int, float> dist_to;       // dist_to[u] is the distance of the shortest s->u path
//     unordered_map<int, int>   previous_node; // previous_node[u] is the vertex before u in the shortest s->u path
//     priority_queue<_pair, vector<_pair>, function<bool(const _pair &, const _pair &)>>
//                               pq([&](const _pair &p1, const _pair &p2)
//                                  { return p1.second > p2.second; });
//     dist_to.insert(make_pair(s, 0)); // set distance of s->s path to 0
//     pq.push(make_pair(s, 0));
//
//     // ====== Backward Dijkstra ======
//     unordered_map<int, float> dist_to_2;       // dist_to_2[u] is the distance of the shortest t->u path
//     unordered_map<int, int>   previous_node_2; // previous_node_2[u] is the node before u in the t->u path
//     priority_queue<_pair, vector<_pair>, function<bool(const _pair &, const _pair &)>>
//                               pq_2([&](const _pair &p1, const _pair &p2)
//                                    { return p1.second > p2.second; });
//     dist_to_2.insert(make_pair(t, 0)); // set distance of t->t path to 0
//     pq_2.push(make_pair(t, 0));
//
//     // If the common_node is guaranteed to be on the shortest path, then we terminate the algorithm
//     int         common_node       = -1; // The current best common node of forward and backward dijkstra
//     float       current_best_dist = numeric_limits<float>::max();
//     while (!pq.empty() && !pq_2.empty()
//            &&
//            dist_to[pq.top().first] + dist_to_2[pq_2.top().first] < current_best_dist)
//     {
//         // ====== Forward Dijkstra ======
//         // Lazy deletion
//         int   u = -1;
//         float dist_to_u;
//         while (!pq.empty())
//         {
//             const auto &top = pq.top();
//             dist_to_u = dist_to[top.first];
//             if (top.second == dist_to_u) // good value
//             {
//                 u = top.first;
//                 pq.pop();
//                 break;
//             }
//             pq.pop();
//         }
//         if (u == -1)
//             break;
//
//         // For every outgoing (u,v) edge
//         for (const auto &e : queryG.GetForwardEdges(u))
//         {
//             const int   v = e.v;
//             const float w = e.w;
//
//             // Relax edge (u,v)
//             const float length    = dist_to_u + w;
//             auto        find_v    = dist_to.find(v);
//             const bool  not_found = find_v == dist_to.end();
//             // If dist_to[v] has not yet been set (its infinity) or length < dist_to[v]
//             if (not_found || length < find_v->second)
//             {
//                 if (not_found)
//                     find_v = dist_to.insert(make_pair(v, length)).first; // set dist_to[v]
//                 else
//                     find_v->second = length;   // update dist_to[v]
//                 pq.push(make_pair(v, length)); // update priority (requires lazy deletion to double pairs)
//                 previous_node[v] = u;          // update previous_node[v]
//             }
//
//             // If vertex v has been visited by backward dijkstra
//             // and it is "better" than the previous common_node, then we update the common_node
//             const auto find_2_v = dist_to_2.find(v);
//             if (find_2_v != dist_to_2.end())
//             {
//                 float dist = find_v->second + find_2_v->second; // dist = dist_to[v] + dist_to_2[v]
//                 if (dist < current_best_dist)
//                 {
//                     common_node       = v;    // update common_node
//                     current_best_dist = dist; // update current_best_dist
//                 }
//             }
//         }
//
//         // ====== Backward Dijkstra ======
//         // Lazy deletion
//         int   u2 = -1;
//         float dist_to_u2;
//         while (!pq_2.empty())
//         {
//             const auto &top = pq_2.top();
//             dist_to_u2 = dist_to_2[top.first];
//             if (top.second == dist_to_u2) // good value
//             {
//                 u2 = top.first;
//                 pq_2.pop();
//                 break;
//             }
//             pq_2.pop();
//         }
//         if (u2 == -1)
//             break;
//
//         // For every outgoing (v,u2) edge in the reverse graph
//         for (const auto &e : queryG.GetBackwardEdges(u2))
//         {
//             const int   v = e.v;
//             const float w = e.w;
//
//             // Relax edge (v,u2)
//             const float length    = dist_to_u2 + w;
//             auto        find_2_v  = dist_to_2.find(v);
//             const bool  not_found = find_2_v == dist_to_2.end();
//             // If dist_to_2[v] has not yet been set (its infinity) or length < dist_to_2[v]
//             if (not_found || length < find_2_v->second)
//             {
//                 if (not_found)
//                     find_2_v = dist_to_2.insert(make_pair(v, length)).first; // set dist_to_2[v]
//                 else
//                     find_2_v->second = length;   // update dist_to_2[v]
//                 pq_2.push(make_pair(v, length)); // update priority (requires lazy deletion to double pairs)
//                 previous_node_2[v] = u2;         // update previous_node_2[v]
//             }
//
//             // If vertex v has been visited by forward dijkstra
//             // and it is "better" than the previous common_node, then we update the common_node
//             const auto find_v = dist_to.find(v);
//             if (find_v != dist_to.end())
//             {
//                 float dist = find_v->second + find_2_v->second;
//                 if (dist < current_best_dist)
//                 {
//                     common_node       = v;    // update common_node
//                     current_best_dist = dist; // update current_best_dist
//                 }
//             }
//         }
//     }
//
//
//     // If there is no s->t path
//     // Update path_cost
//     if (common_node == -1)
//     {
//         path_cost = numeric_limits<float>::max();
//         return;
//     }
//
//     // Update path_cost
//     path_cost = current_best_dist;
//
//     // Calculate the path vector
//     //
//     // ====== Forward Dijkstra ======
//     if (common_node != s)
//     {
//         for (int prev = common_node; (prev = previous_node[prev]) != s;) // keep getting the previous vertex
//             shortest_path.push_back(prev);
//         shortest_path.push_back(s); // add first vertex
//     }
//     // ====== Backward Dijkstra ======
//     vector<int> backward_dijkstra_path;
//     if (common_node != t)
//     {
//         for (int prev = common_node; (prev = previous_node_2[prev]) != t;) // keep getting the previous vertex
//             backward_dijkstra_path.push_back(prev);
//         backward_dijkstra_path.push_back(t); // add first vertex
//     }
//     // Combine Forward & Backward Dijkstra
//     reverse(shortest_path.begin(), shortest_path.end()); // reverse the vector
//     shortest_path.push_back(common_node);
//     shortest_path.insert(shortest_path.end(), backward_dijkstra_path.begin(), backward_dijkstra_path.end());
// }
//
// ShortestPath::~ShortestPath()
//{
//
// }
