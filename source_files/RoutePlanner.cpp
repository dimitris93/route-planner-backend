#include "RoutePlanner.h"
#include <functional>
#include <queue>
#include <unordered_map>

Route RoutePlanner::plan_route(unsigned int s,
							   unsigned int t,
							   const Graph& G,
							   Algorithm    algorithm)
{
	vector<unsigned int> shortest_path;
	float                path_cost;

	switch (algorithm)
	{
		case Dijkstra:
		{
			// s->s path is simply {s} and it has 0 cost
			if (s == t)
			{
				shortest_path.push_back(s);
				return Route(shortest_path, path_cost);
			}

			unordered_map<unsigned int, float>        dist;   // dist[u] is the distance of the shortest s->u path
			unordered_map<unsigned int, unsigned int> prev;   // prev[u] is the node before u in the shortest s->u path
			using ui_f = pair<unsigned int, float>;
			priority_queue<ui_f,
						   vector<ui_f>,
						   function<bool(const ui_f&, const ui_f&)>>
					pq([&](const ui_f& p1, const ui_f& p2) {
						return p1.second > p2.second;
					});

			dist.insert(make_pair(s, 0));   // set distance of s->s path to 0
			pq.push(make_pair(s, 0));       // add s to pq

			// While our priority queue is not empty
			while (!pq.empty())
			{
				// Lazy deletion
				int   u = -1;
				float dist_to_u;
				while (!pq.empty())
				{
					const auto& top = pq.top();
					dist_to_u       = dist[top.first];
					if (top.second == dist_to_u)   // good value
					{
						u = top.first;
						pq.pop();
						break;
					}
					pq.pop();
				}
				if (u == -1)
					break;

				// Stop the algorithm because the shortest s->t path is found, since the vertex t was popped
				if (u == t)
					break;

				// For every outgoing (u,v) edge
				for (const auto& e : G.GetForwardEdges(u))
				{
					const unsigned int v = e.v;
					const float        w = e.w;

					// Relax edge (u,v)
					const float length    = dist_to_u + w;
					const auto  find_v    = dist.find(v);
					const bool  not_found = find_v == dist.end();
					// If dist_to[v] has not yet been set or length < dist_to[v]
					if (not_found || length < find_v->second)
					{
						if (not_found)
							dist.insert(make_pair(v, length));   // set dist_to[v]
						else
							find_v->second = length;     // update dist_to[v]
						pq.push(make_pair(v, length));   // update priority (requires lazy deletion to double pairs)
						prev[v] = u;                     // update previous_node[v]
					}
				}
			}

			// If there is no s->t path
			// Update path_cost
			const auto find_t = dist.find(t);
			if (find_t == dist.end())
			{
				path_cost = numeric_limits<float>::max();
				return Route(shortest_path, path_cost);
			}

			// Update path_cost
			path_cost = find_t->second;

			// Calculate the path vector
			shortest_path.push_back(t);            // add last vertex
			for (int p = t; (p = prev[p]) != s;)   // keep getting the previous vertex
				shortest_path.push_back(p);
			shortest_path.push_back(s);                            // add first vertex
			reverse(shortest_path.begin(), shortest_path.end());   // reverse the vector

			cout << shortest_path.size() << endl;
			return Route(shortest_path, path_cost);
		}
		case Bidirectional_Dijkstra:
		{
			return Route(shortest_path, path_cost);
		}
	}
}

//
//ShortestPath::ShortestPath(const QueryGraph &queryG,
//                           vector<int> &shortest_path,
//                           float &path_cost,
//                           Algorithm algorithm)
//{
//    switch (algorithm)
//    {
//        case Dijkstra:
//            SimpleDijkstra(queryG, shortest_path, path_cost);
//            break;
//        case Bidirectional_Dijkstra:
//            BidirectionalDijkstra(queryG, shortest_path, path_cost);
//            break;
//    }
//}
//
//void ShortestPath::SimpleDijkstra(const QueryGraph &queryG, vector<int> &shortest_path, float &path_cost)
//{
//    const int s = queryG.from;
//    const int t = queryG.to;
//
//    // s->s path is simply {s} and it has 0 cost
//    if (s == t)
//    {
//        shortest_path.push_back(s);
//        path_cost = 0;
//        return;
//    }
//
//    unordered_map<int, float> dist_to;       // dist_to[u] is the distance of the shortest s->u path
//    unordered_map<int, int>   previous_node; // previous_node[u] is the node before u in the shortest s->u path
//    using _pair = pair<int, float>;
//    priority_queue<_pair, vector<_pair>, function<bool(const _pair &, const _pair &)>>
//                              pq([&](const _pair &p1, const _pair &p2)
//                                 { return p1.second > p2.second; });
//    dist_to.insert(make_pair(s, 0)); // set distance of s->s path to 0
//    pq.push(make_pair(s, 0));
//
//    // While our priority queue is not empty
//    while (!pq.empty())
//    {
//        // Lazy deletion
//        int   u = -1;
//        float dist_to_u;
//        while (!pq.empty())
//        {
//            const auto &top = pq.top();
//            dist_to_u = dist_to[top.first];
//            if (top.second == dist_to_u) // good value
//            {
//                u = top.first;
//                pq.pop();
//                break;
//            }
//            pq.pop();
//        }
//        if (u == -1)
//            break;
//
//        // Stop the algorithm because the shortest s->t path is found, since the vertex t was popped
//        if (u == t)
//            break;
//
//        // For every outgoing (u,v) edge
//        for (const auto &e : queryG.GetForwardEdges(u))
//        {
//            const int   v = e.v;
//            const float w = e.w;
//
//            // Relax edge (u,v)
//            const float length    = dist_to_u + w;
//            const auto  find_v    = dist_to.find(v);
//            const bool  not_found = find_v == dist_to.end();
//            // If dist_to[v] has not yet been set or length < dist_to[v]
//            if (not_found || length < find_v->second)
//            {
//                if (not_found)
//                    dist_to.insert(make_pair(v, length)); // set dist_to[v]
//                else
//                    find_v->second = length;   // update dist_to[v]
//                pq.push(make_pair(v, length)); // update priority (requires lazy deletion to double pairs)
//                previous_node[v] = u;          // update previous_node[v]
//            }
//        }
//    }
//
//
//    // If there is no s->t path
//    // Update path_cost
//    const auto find_t = dist_to.find(t);
//    if (find_t == dist_to.end())
//    {
//        path_cost = numeric_limits<float>::max();
//        return;
//    }
//
//    // Update path_cost
//    path_cost = find_t->second;
//
//    // Calculate the path vector
//    shortest_path.push_back(t); // add last vertex
//    for (int prev = t; (prev = previous_node[prev]) != s;) // keep getting the previous vertex
//        shortest_path.push_back(prev);
//    shortest_path.push_back(s); // add first vertex
//    reverse(shortest_path.begin(), shortest_path.end()); // reverse the vector
//}
//
//void ShortestPath::BidirectionalDijkstra(const QueryGraph &queryG, vector<int> &shortest_path, float &path_cost)
//{
//    const int s = queryG.from;
//    const int t = queryG.to;
//
//    // s->s path is simply {s} and it has 0 cost
//    if (s == t)
//    {
//        shortest_path.push_back(s);
//        path_cost = 0;
//        return;
//    }
//
//    using _pair = pair<int, float>;
//
//    // ====== Forward Dijkstra ======
//    unordered_map<int, float> dist_to;       // dist_to[u] is the distance of the shortest s->u path
//    unordered_map<int, int>   previous_node; // previous_node[u] is the vertex before u in the shortest s->u path
//    priority_queue<_pair, vector<_pair>, function<bool(const _pair &, const _pair &)>>
//                              pq([&](const _pair &p1, const _pair &p2)
//                                 { return p1.second > p2.second; });
//    dist_to.insert(make_pair(s, 0)); // set distance of s->s path to 0
//    pq.push(make_pair(s, 0));
//
//    // ====== Backward Dijkstra ======
//    unordered_map<int, float> dist_to_2;       // dist_to_2[u] is the distance of the shortest t->u path
//    unordered_map<int, int>   previous_node_2; // previous_node_2[u] is the node before u in the t->u path
//    priority_queue<_pair, vector<_pair>, function<bool(const _pair &, const _pair &)>>
//                              pq_2([&](const _pair &p1, const _pair &p2)
//                                   { return p1.second > p2.second; });
//    dist_to_2.insert(make_pair(t, 0)); // set distance of t->t path to 0
//    pq_2.push(make_pair(t, 0));
//
//    // If the common_node is guaranteed to be on the shortest path, then we terminate the algorithm
//    int         common_node       = -1; // The current best common node of forward and backward dijkstra
//    float       current_best_dist = numeric_limits<float>::max();
//    while (!pq.empty() && !pq_2.empty()
//           &&
//           dist_to[pq.top().first] + dist_to_2[pq_2.top().first] < current_best_dist)
//    {
//        // ====== Forward Dijkstra ======
//        // Lazy deletion
//        int   u = -1;
//        float dist_to_u;
//        while (!pq.empty())
//        {
//            const auto &top = pq.top();
//            dist_to_u = dist_to[top.first];
//            if (top.second == dist_to_u) // good value
//            {
//                u = top.first;
//                pq.pop();
//                break;
//            }
//            pq.pop();
//        }
//        if (u == -1)
//            break;
//
//        // For every outgoing (u,v) edge
//        for (const auto &e : queryG.GetForwardEdges(u))
//        {
//            const int   v = e.v;
//            const float w = e.w;
//
//            // Relax edge (u,v)
//            const float length    = dist_to_u + w;
//            auto        find_v    = dist_to.find(v);
//            const bool  not_found = find_v == dist_to.end();
//            // If dist_to[v] has not yet been set (its infinity) or length < dist_to[v]
//            if (not_found || length < find_v->second)
//            {
//                if (not_found)
//                    find_v = dist_to.insert(make_pair(v, length)).first; // set dist_to[v]
//                else
//                    find_v->second = length;   // update dist_to[v]
//                pq.push(make_pair(v, length)); // update priority (requires lazy deletion to double pairs)
//                previous_node[v] = u;          // update previous_node[v]
//            }
//
//            // If vertex v has been visited by backward dijkstra
//            // and it is "better" than the previous common_node, then we update the common_node
//            const auto find_2_v = dist_to_2.find(v);
//            if (find_2_v != dist_to_2.end())
//            {
//                float dist = find_v->second + find_2_v->second; // dist = dist_to[v] + dist_to_2[v]
//                if (dist < current_best_dist)
//                {
//                    common_node       = v;    // update common_node
//                    current_best_dist = dist; // update current_best_dist
//                }
//            }
//        }
//
//        // ====== Backward Dijkstra ======
//        // Lazy deletion
//        int   u2 = -1;
//        float dist_to_u2;
//        while (!pq_2.empty())
//        {
//            const auto &top = pq_2.top();
//            dist_to_u2 = dist_to_2[top.first];
//            if (top.second == dist_to_u2) // good value
//            {
//                u2 = top.first;
//                pq_2.pop();
//                break;
//            }
//            pq_2.pop();
//        }
//        if (u2 == -1)
//            break;
//
//        // For every outgoing (v,u2) edge in the reverse graph
//        for (const auto &e : queryG.GetBackwardEdges(u2))
//        {
//            const int   v = e.v;
//            const float w = e.w;
//
//            // Relax edge (v,u2)
//            const float length    = dist_to_u2 + w;
//            auto        find_2_v  = dist_to_2.find(v);
//            const bool  not_found = find_2_v == dist_to_2.end();
//            // If dist_to_2[v] has not yet been set (its infinity) or length < dist_to_2[v]
//            if (not_found || length < find_2_v->second)
//            {
//                if (not_found)
//                    find_2_v = dist_to_2.insert(make_pair(v, length)).first; // set dist_to_2[v]
//                else
//                    find_2_v->second = length;   // update dist_to_2[v]
//                pq_2.push(make_pair(v, length)); // update priority (requires lazy deletion to double pairs)
//                previous_node_2[v] = u2;         // update previous_node_2[v]
//            }
//
//            // If vertex v has been visited by forward dijkstra
//            // and it is "better" than the previous common_node, then we update the common_node
//            const auto find_v = dist_to.find(v);
//            if (find_v != dist_to.end())
//            {
//                float dist = find_v->second + find_2_v->second;
//                if (dist < current_best_dist)
//                {
//                    common_node       = v;    // update common_node
//                    current_best_dist = dist; // update current_best_dist
//                }
//            }
//        }
//    }
//
//
//    // If there is no s->t path
//    // Update path_cost
//    if (common_node == -1)
//    {
//        path_cost = numeric_limits<float>::max();
//        return;
//    }
//
//    // Update path_cost
//    path_cost = current_best_dist;
//
//    // Calculate the path vector
//    //
//    // ====== Forward Dijkstra ======
//    if (common_node != s)
//    {
//        for (int prev = common_node; (prev = previous_node[prev]) != s;) // keep getting the previous vertex
//            shortest_path.push_back(prev);
//        shortest_path.push_back(s); // add first vertex
//    }
//    // ====== Backward Dijkstra ======
//    vector<int> backward_dijkstra_path;
//    if (common_node != t)
//    {
//        for (int prev = common_node; (prev = previous_node_2[prev]) != t;) // keep getting the previous vertex
//            backward_dijkstra_path.push_back(prev);
//        backward_dijkstra_path.push_back(t); // add first vertex
//    }
//    // Combine Forward & Backward Dijkstra
//    reverse(shortest_path.begin(), shortest_path.end()); // reverse the vector
//    shortest_path.push_back(common_node);
//    shortest_path.insert(shortest_path.end(), backward_dijkstra_path.begin(), backward_dijkstra_path.end());
//}
//
//ShortestPath::~ShortestPath()
//{
//
//}
