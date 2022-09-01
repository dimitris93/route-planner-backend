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

    // Find the nearest edge
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
        // Update the nearest edge information
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
