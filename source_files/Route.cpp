#include "Route.h"
#include <functional>
#include <queue>
#include <unordered_map>

Route::Route(const Graph&     G,
			 nodeid_t         s,
			 nodeid_t         t,
			 Route::Algorithm algorithm) :
	algorithm(algorithm)
{
	switch (algorithm)
	{
		case Dijkstra:
		{
			unordered_map<nodeid_t, float>    dist;   // dist[u] 'will be' the distance of the shortest s->u path
			unordered_map<nodeid_t, nodeid_t> prev;   // prev[x] 'will be' the node before x in the shortest s->u path, via x
			using id_w_pair = pair<nodeid_t, float>;
			priority_queue<id_w_pair,
						   vector<id_w_pair>,
						   function<bool(const id_w_pair&, const id_w_pair&)>>
					pq([&](const id_w_pair& p1, const id_w_pair& p2) {
						return p1.second > p2.second;
					});

			dist[s] = 0;                // set distance of s->s path to 0
			pq.push(make_pair(s, 0));   // add s to pq

			while (true)
			{
				// Nodes can be added more than once in the priority queue, for efficiency.
				// We need to remove them by checking if they are up-to-date.
				// This is common for Dijkstra implementations.
				while (!pq.empty() &&
					   pq.top().second > dist[pq.top().first])
				{
					pq.pop();
				}

				// Terminate condition. An s->t path shorter than dist[t] could not have existed beyond this point.
				if (pq.empty() ||
					pq.top().first == t)
				{
					break;
				}

				// Pop node u from priority queue, and visit all of its outgoing edges
				const nodeid_t u = pq.top().first;
				pq.pop();
				for (const auto& e : G.GetAdjacentEdges(u))
				{
					const float len           = dist[u] + e.w;
					const auto  find_dist_e_v = dist.find(e.v);
					// If dist[e.v] is Infinite, or dist[u] + e.w < dist[e.v]
					if (find_dist_e_v == dist.end() || len < find_dist_e_v->second)
					{
						// "Relax" edge e.v (relax is a term that is often used in explaining Dijkstra's algorithm)
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
				break;   // no route
			}

			// Calculate the path vector
			vector<nodeid_t> shortest_path;
			nodeid_t         p = t;
			do
			{
				shortest_path.push_back(p);
				p = prev[p];
			} while (p != s);
			shortest_path.push_back(s);                            // add first node
			reverse(shortest_path.begin(), shortest_path.end());   // reverse the vector

			nodeid_sequence = shortest_path;
			sum_weight      = dist[t];
		}
		case Bidirectional_Dijkstra:
		{
			// Forward Dijkstra
			unordered_map<nodeid_t, float>    dist;   // dist[u] 'will be' the distance of the shortest s->u path
			unordered_map<nodeid_t, nodeid_t> prev;   // prev[x] 'will be' the node before x in the shortest s->u path, via x
			using ui_f = pair<nodeid_t, float>;
			priority_queue<ui_f,
						   vector<ui_f>,
						   function<bool(const ui_f&, const ui_f&)>>
					pq([&](const ui_f& p1, const ui_f& p2) {
						return p1.second > p2.second;
					});

			dist[s] = 0;                // set distance of s->s path to 0
			pq.push(make_pair(s, 0));   // add s to pq

			// Backward Dijkstra
			unordered_map<nodeid_t, float>    dist2;   // dist2[u] 'will be' the distance of the shortest u<-t path
			unordered_map<nodeid_t, nodeid_t> prev2;   // prev2[x] 'will be' the node before x in the shortest u<-t path, via x
			using ui_f = pair<nodeid_t, float>;
			priority_queue<ui_f,
						   vector<ui_f>,
						   function<bool(const ui_f&, const ui_f&)>>
					pq2([&](const ui_f& p1, const ui_f& p2) {
						return p1.second > p2.second;
					});

			dist2[t] = 0;                // set distance of t->t path to 0
			pq2.push(make_pair(t, 0));   // add t to pq2

			nodeid_t common_node = -1;
			float    best_dist   = numeric_limits<float>::max();
			while (true)
			{
				// Forward Dijkstra
				while (!pq.empty() &&
					   pq.top().second > dist[pq.top().first])
				{
					pq.pop();   // delete if node was added more than once (value not up-to-date)
				}
				// Backward Dijkstra
				while (!pq2.empty() &&
					   pq2.top().second > dist2[pq2.top().first])
				{
					pq2.pop();   // delete if node was added more than once (value not up-to-date)
				}

				// Terminate condition. An s->t path shorter than best_dist could not have existed beyond this point.
				if (pq.empty() ||
					pq2.empty() ||
					dist[pq.top().first] + dist2[pq2.top().first] >= best_dist)
				{
					break;
				}

				// Forward Dijkstra
				nodeid_t u = pq.top().first;
				pq.pop();
				for (const auto& e : G.GetAdjacentEdges(u))
				{
					const float len           = dist[u] + e.w;
					const auto  find_dist_e_v = dist.find(e.v);
					// If dist[e.v] is Infinite, or dist[u] + e.w < dist[e.v]
					if (find_dist_e_v == dist.end() || len < find_dist_e_v->second)
					{
						// Relax edge e
						dist[e.v] = len;
						prev[e.v] = u;
						pq.push(make_pair(e.v, len));
					}

					// If node e.v has been visited by backward Dijkstra,
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
				u = pq2.top().first;
				pq2.pop();
				for (const auto& e : G.GetAdjacentEdges(u, true))
				{
					const float len            = dist2[u] + e.w;
					const auto  find_dist2_e_v = dist2.find(e.v);
					// If dist2[e.v] is Infinite, or dist2[u] + e.w < dist2[e.v]
					if (find_dist2_e_v == dist2.end() || len < find_dist2_e_v->second)
					{
						// Relax edge e
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
				break;
			}

			vector<nodeid_t> shortest_path;

			// Calculate the path vector by combining the shortest path from s -> c and c <- t
			if (common_node != s)
			{
				nodeid_t p = common_node;

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
				nodeid_t p = common_node;

				do
				{
					p = prev2[p];
					shortest_path.push_back(p);
				} while (p != t);
			}

			nodeid_sequence = shortest_path;
			sum_weight      = best_dist;
		}
	}
}
