#include "GraphStorage.h"
#include <iostream>
#include <unordered_set>
#include <set>
#include <functional>
#include <stack>
#include <ShortestPath.h>
#include <queue>

using namespace std;

GraphStorage::GraphStorage(int vertices, int max_edge_segment_length) :
    kd_tree(coordinates),
    coordinates(vertices),
    adjacency_lists(vertices),
    node_to_connected_component_id(vertices, -1),
    current_component_id(0),
    directed_edges_counter(0),
    MAX_EDGE_SEGMENT_LENGTH(max_edge_segment_length)
{

}

GraphStorage::~GraphStorage()
{

}

int GraphStorage::GetNodes() const
{
    return coordinates.size();
}

int GraphStorage::GetEdges() const
{
    return directed_edges_counter;
}

void GraphStorage::SetCoordinate(int node_id, const double &lat, const double &lng)
{
    coordinates[node_id] = LatLng(lat, lng);
}

void GraphStorage::AddEdge(int u, int v, float w, bool double_way, bool is_startpoint)
{
    if (double_way)
    {
        adjacency_lists[u].push_back(AdjListElement(v, w, Edge::DOUBLE_WAY_EDGE, is_startpoint));
        adjacency_lists[v].push_back(AdjListElement(u, w,Edge::DOUBLE_WAY_EDGE, is_startpoint));
        directed_edges_counter += 2;
    }
    else
    {
        adjacency_lists[u].push_back(AdjListElement(v, w, Edge::FORWARD_EDGE, is_startpoint));
        adjacency_lists[v].push_back(AdjListElement(u, w, Edge::BACKWARD_EDGE, is_startpoint));
        directed_edges_counter++;
    }
}

void GraphStorage::CreateKDTree()
{
    kd_tree.Create(); // create the KD Tree from our coordinates vector
}

const LatLng &GraphStorage::GetCoordinate(int node_id) const
{
    return coordinates[node_id];
}

NearestEdgeInfo GraphStorage::GetNearestEdge(const LatLng &source, const double &radius) const
{
    // Get all nodes within a radius
    vector<int> nodes = kd_tree.RadiusSearch(source, radius);

    // Get all edges within that radius
    // We want to get a set of edges and avoid adding
    // the same A->B and B->A edge twice
    set<Edge, function<bool(const Edge &, const Edge &)>>
                edges(
                    // Custom compare function to avoid adding A->B and B->A edges twice
                    [](const Edge &e1, const Edge &e2)
                    {
                        const int x = min(e1.u, e1.v);
                        const int y = min(e2.u, e2.v);
                        if (x < y)
                            return true;
                        else if (y > x)
                            return false;
                        else
                            return max(e1.u, e1.v) < max(e2.u, e2.v);
                    }
                );

    // Fill in a set of (undirected) edges
    // For each node
    for (int i = 0; i < nodes.size(); ++i)
    {
        const int A = nodes[i];

        // For each edge that starts from this node
        for (int j = 0; j < adjacency_lists[A].size(); ++j)
        {
            const auto &e = adjacency_lists[A][j];
            if (e.is_startpoint) // only startpoints
            {
                const int B = e.v;
                if (e.edge_type == Edge::BACKWARD_EDGE)
                    edges.insert(Edge(B, A, e.w, e.edge_type));
                else
                    edges.insert(Edge(A, B, e.w, e.edge_type));
            }
        }
    }

    auto current_best = NearestEdgeInfo();

    if (edges.empty())
    {
        return current_best;
    }

    // Find nearest edge
    const double    source_lat    = source.getLat();
    const double    source_lng    = source.getLng();
    double          best_distance = numeric_limits<double>::max();
    for (const auto &e : edges)
    {
        double clamped_ratio;
        LatLng projected_point;

        tie(clamped_ratio, projected_point) = LatLng::ProjectOnSegment(source_lat, source_lng,
                                                                       coordinates[e.u], coordinates[e.v]);
        const double current_distance = LatLng::DistanceInMeters(source_lat, source_lng,
                                                                 projected_point.getLat(), projected_point.getLng());
        // Update nearest edge information
        if (current_distance < best_distance)
        {
            best_distance = current_distance;
            current_best  = NearestEdgeInfo(e, projected_point, clamped_ratio, true);
        }
    }
    return current_best;
}

vector<AdjacentEdge> GraphStorage::GetForwardEdges(int u) const
{
    vector<AdjacentEdge> list;
    for (const auto      &e : adjacency_lists[u])
    {
        if (e.edge_type != Edge::BACKWARD_EDGE)
            list.push_back(AdjacentEdge(e.v, e.w));
    }
    return list;
}

vector<AdjacentEdge> GraphStorage::GetBackwardEdges(int u) const
{
    vector<AdjacentEdge> list;
    for (const auto      &e : adjacency_lists[u])
    {
        if (e.edge_type != Edge::FORWARD_EDGE)
            list.push_back(AdjacentEdge(e.v, e.w));
    }
    return list;
}

Edge GraphStorage::FindForwardEdge(int u, int v) const
{
    const auto find_edge_uv = find_if(adjacency_lists[u].begin(), adjacency_lists[u].end(),
                                      [&](const AdjListElement &e)
                                      {
                                          return e.v == v && e.edge_type == Edge::FORWARD_EDGE;
                                      });
    if (find_edge_uv != adjacency_lists[u].end())
    {
        return Edge(u, (*find_edge_uv).v, (*find_edge_uv).w, (*find_edge_uv).edge_type);
    }
    return Edge();
}

void GraphStorage::FindConnectedComponents()
{
    // Iterative DFS
    // (recursion would result in stack overflow)
    for (int i = 0; i < coordinates.size(); ++i)
    {
        // We already visited node this node, so continue
        if (node_to_connected_component_id[i] != -1)
            continue;

        stack<int> stack;
        // Push node i to stack and mark it as visited
        stack.push(i);
        node_to_connected_component_id[i] = current_component_id;

        while (!stack.empty())
        {
            int u = stack.top();
            stack.pop();

            // For every unvisited node v adjacent to u
            // Push node v to stack and mark it as visited
            for (const auto &e : adjacency_lists[u])
            {
                if (node_to_connected_component_id[e.v] == -1)
                {
                    stack.push(e.v);
                    node_to_connected_component_id[e.v] = current_component_id;
                }
            }
        }
        current_component_id++;
    }
    cout << current_component_id << " connected components.\n";
}

bool GraphStorage::AreConnected(int u, int v) const
{
    return node_to_connected_component_id[u] ==
           node_to_connected_component_id[v];
}

//void GraphStorage::ComputeContractionHierarchies()
//{
//    int           current_new_node_id = 0;
//    vector<int>   old_to_new_id(coordinates.size(), -1);
//    vector<float> priorities(coordinates.size());
//
//    PQ pq([&](const _pair &p1, const _pair &p2)
//          { return p1.second > p2.second; });
//
//    // Weight of the terms, when calculating the priorities
//    // a * (#shortcuts that would be added - #adjacent edges)
//    // + b * (#deleted neighbours)
//    const float a = 0.75;
//    const float b = 0.25;
//
//    // Calculate initial priorities
//    for (int v = 0; v < coordinates.size(); ++v)
//    {
//        update_ch_priorities(v, a, b, pq, priorities, old_to_new_id, true);
//    }
////    priorities[0] = -1;
////    priorities[1] = 1;
////    priorities[2] = 2;
////    priorities[3] = 3;
////    priorities[4] = 4;
////    priorities[5] = 5;
////    pq.push(make_pair(0, -1));
//
//    for (int v = 0; v < coordinates.size(); ++v)
//    {
//        pq.push(make_pair(v, priorities[v]));
//    }
//
//    int total_directed_shortcuts = 0;
//    while (!pq.empty())
//    {
//        // Lazy deletion
//        int v = -1;
//        while (!pq.empty())
//        {
//            const auto &top = pq.top();
//            if (top.second == priorities[top.first]) // good value
//            {
//                v = top.first;
//                pq.pop();
//                break;
//            }
//            pq.pop();
//        }
//        if (v == -1)
//            break;
//
////        cout << "popped " << v << endl;
////        cout << v << " " << old_to_new_id[v] << endl;
//        old_to_new_id[v] = current_new_node_id;
////        cout << v << "->" << old_to_new_id[v] << endl;
////        cout << pq.size() << endl;
////        cout << v << endl;
//        current_new_node_id++;
//
//        vector<AdjacentEdge> vw_edges;
//        for (const auto      &e: adjacency_lists[v])
//        {
//            if (e.edge_type == Edge::BACKWARD_EDGE || old_to_new_id[e.v] != -1)
//                continue;
//
//            vw_edges.push_back(AdjacentEdge(e.v, e.w));
//        }
//
//        if (vw_edges.empty())
//        {
//            // Update priorities of neighbours if they are not contracted
//            for (const auto &e: adjacency_lists[v])
//            {
//                if (old_to_new_id[e.v] == -1)
//                    update_ch_priorities(e.v, a, b, pq, priorities, old_to_new_id, false);
//            }
//            continue;
//        }
//
//
////        if (v == 3085)
////        {
////            cout << "---- uv edges" << endl;
////            for (const auto &e: adjacency_lists[v])
////            {
////                if (e.edge_type == Edge::FORWARD_EDGE || old_to_new_id[e.v] != -1)
////                    continue;
////                cout << e.v << " " << v << " " << e.edge_type << " " << e.middle_node << endl;
////            }
////            cout << "---- vw edges" << endl;
////            for (const auto &e: adjacency_lists[v])
////            {
////                if (e.edge_type == Edge::BACKWARD_EDGE || old_to_new_id[e.v] != -1)
////                    continue;
////                cout << v << " " << e.v << " " << e.edge_type << " " << e.middle_node << endl;
////            }
////        }
//
//
////        for (const auto      &e: adjacency_lists[v])
////        {
////            if (e.edge_type == Edge::BACKWARD_EDGE || old_to_new_id[e.v] != -1)
////                continue;
////
////            for (int j = 0; j < vw_edges.size(); ++j)
////            {
////                cout << e.v << " " << v << " " << vw_edges[j].v << endl;
////            }
////        }
//
//
//        for (const auto &e: adjacency_lists[v])
//        {
//            if (e.edge_type == Edge::FORWARD_EDGE || old_to_new_id[e.v] != -1)
//                continue;
//
//            const int            u         = e.v;
//            const float          uv_cost   = e.w;
//            vector<AdjacentEdge> shortcuts = vw_edges;
//            ShortestPath         sp(u, v, uv_cost, shortcuts, adjacency_lists);
//
//            for (int i = 0; i < shortcuts.size(); ++i)
//            {
//                const int   w    = shortcuts[i].v;
//                const float cost = uv_cost + shortcuts[i].w;
//                total_directed_shortcuts++;
//
//
//                const auto find_edge_uw = find_if(adjacency_lists[u].begin(), adjacency_lists[u].end(),
//                                                  [&](const AdjListElement &e)
//                                                  {
//                                                      return e.v == w &&
//                                                             e.middle_node == v &&
//                                                             e.edge_type == Edge::FORWARD_EDGE;
//                                                  });
//                if (find_edge_uw != adjacency_lists[u].end())
//                {
//                    cout << "found: " << u << " " << v << " " << (*find_edge_uw).v << endl;
//                }
//                adjacency_lists[u].push_back(AdjListElement(w, cost, v, Edge::FORWARD_EDGE, false));
//                adjacency_lists[w].push_back(AdjListElement(u, cost, v, Edge::BACKWARD_EDGE, false));
//
////                cout << u << " " << v << " " << w << " cost: " << cost << endl;
////                cout << "-1-- " << u << " " << v << " " << w << endl;
////                cout << "-2-- " << old_to_new_id[u] << " " << old_to_new_id[v] << " " << old_to_new_id[w] << endl;
//            }
//        }
//
//        // Update priorities of neighbours if they are not contracted
//        for (const auto &e: adjacency_lists[v])
//        {
//            if (old_to_new_id[e.v] == -1)
//                update_ch_priorities(e.v, a, b, pq, priorities, old_to_new_id, false);
//        }
//
////        cout << "Total directed shortcut edges: " << total_directed_shortcuts << "\n";
//    }
//    cout << "Total directed shortcut edges: " << total_directed_shortcuts << "\n";
//}
//void GraphStorage::update_ch_priorities(int v,
//                                        float a, float b,
//                                        PQ &pq,
//                                        vector<float> &priorities,
//                                        vector<int> &old_to_new_id,
//                                        bool initialize_priority)
//{
//    vector<AdjacentEdge> vw_edges;
//    for (const auto      &e: adjacency_lists[v])
//    {
//        if (e.edge_type == Edge::BACKWARD_EDGE || old_to_new_id[e.v] != -1)
//            continue;
//
//        vw_edges.push_back(AdjacentEdge(e.v, e.w));
//    }
//
//    if (vw_edges.empty())
//        return;
//
//    int             shortcuts_count = 0;
//    for (const auto &e: adjacency_lists[v])
//    {
//        if (e.edge_type == Edge::FORWARD_EDGE || old_to_new_id[e.v] != -1)
//            continue;
//
//        const int            u         = e.v;
//        const float          uv_cost   = e.w;
//        vector<AdjacentEdge> shortcuts = vw_edges;
//        ShortestPath         sp(u, v, uv_cost, shortcuts, adjacency_lists);
//        shortcuts_count += shortcuts.size();
//    }
//
//    const int adjacent_edges_count = adjacency_lists[v].size();
//    const int edge_difference      = shortcuts_count - adjacent_edges_count;
//
//    // Initialize priority
//    if (initialize_priority)
//    {
//        priorities[v] = a * edge_difference;
////        cout << "init: " << v << " " << priorities[v] << endl;
////        cout << v << " shortcuts: " << shortcuts_count << " adj_edges: " << adjacent_edges_count << endl;
////        cout << "recap above ---" << endl;
//    }
//        // Update priority if node hasn't been contracted yet
//    else
//    {
//        int             deleted_neighbours = 0;
//        for (const auto &e: adjacency_lists[v])
//        {
//            // If node has been contracted
//            if (old_to_new_id[e.v] != -1)
//                deleted_neighbours++;
//        }
//        const float     previous_priority  = priorities[v];
//        priorities[v] = a * edge_difference + b * deleted_neighbours;
////        cout <<
////        "update: " << v <<
////        " priority " << priorities[v] <<
////        " shortcuts_count " << shortcuts_count <<
////        " adjacent_edges_count " << adjacent_edges_count <<
////        " deleted_neighbours " << deleted_neighbours << endl;
////        if (shortcuts_count > 22)
////            cout << "update: " << v << " " << shortcuts_count << endl;
////        cout << shortcuts_count << endl;
//        if (previous_priority != priorities[v])
//            pq.push(make_pair(v, priorities[v]));
//    }
//}