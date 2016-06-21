#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <functional>
#include <set>
#include <queue>
#include "KDTree.h"

#ifndef GRAPH_STORAGE_H
#define GRAPH_STORAGE_H

using namespace std;

struct AdjacentEdge
{
    int   v;
    float w;

    AdjacentEdge(int v, float w) :
        v(v),
        w(w)
    { }
};

struct Edge
{
    int   u, v;
    float w;
    char  type;
    bool  is_startpoint;

    static const char FORWARD_EDGE    = 'F';
    static const char BACKWARD_EDGE   = 'B';
    static const char DOUBLE_WAY_EDGE = 'D';
    static const char NULL_EDGE       = 'E';

    Edge() :
        type(Edge::NULL_EDGE)
    { }

    Edge(int u, int v, float w, char type) :
        u(u),
        v(v),
        w(w),
        type(type)
    { }
};

struct NearestEdgeInfo
{
    Edge   edge;
    LatLng projected_point;
    double clamped_ratio;
    bool   edge_found;

    NearestEdgeInfo() :
        edge_found(false)
    { }

    NearestEdgeInfo(Edge edge, LatLng projected_point, double clamped_ratio, bool edge_found) :
        edge(edge),
        projected_point(projected_point),
        clamped_ratio(clamped_ratio),
        edge_found(edge_found)
    { }
};

struct AdjListElement
{
    int   v;
    float w;
    char  edge_type;
    bool  is_startpoint;

    AdjListElement(int v, float w,
                   char edge_type, bool is_startpoint) :
        v(v),
        w(w),
        edge_type(edge_type),
        is_startpoint(is_startpoint)
    { }
};

/*
 * Fixme: update comments
 * Directed Edge-Weighted GraphStorage implementation using Adjacency Lists.
 * The adjacency lists are implemented as std::vectors
 * and they are all stored inside an outer std::vector.
 * We use the node ID as index to retrieve the adjacency list of that node.
 */
class GraphStorage
{
public:
    GraphStorage(int vertices,
                 int max_edge_segment_length);                       // Create GraphStorage and allocate memory
    ~GraphStorage();                                                 // Destroy GraphStorage

    int                  GetNodes() const;                           // Number of nodes
    int                  GetEdges() const;                           // Number of directed edges
    void                 SetCoordinate(int node_id,
                                       const double &lat,
                                       const double &lng);           // Add coordinate (lat,lng)
    void                 AddEdge(int u, int v, float w,
                                 bool double_way,
                                 bool is_startpoint);                // Add edge (u,v) with w weight
    void                 CreateKDTree();                             // Create KD Tree from the coordinates
    void                 FindConnectedComponents();                  // Create KD Tree from the coordinates
    void                 ComputeContractionHierarchies();            // Create KD Tree from the coordinates
    const LatLng         &GetCoordinate(int node_id) const;          // Get the latitude,longitude of a node
    NearestEdgeInfo      GetNearestEdge(const LatLng &source,
                                        const double &radius) const; // Nearest edge from coordinate
    vector<AdjacentEdge> GetForwardEdges(int u) const;               // Forward adjacent edges of the vertex u
    vector<AdjacentEdge> GetBackwardEdges(int u) const;              // Backward adjacent edges of the vertex u
    Edge                 FindForwardEdge(int u, int v) const;        // Returns edge if it exists
    bool                 AreConnected(int u, int v) const;           // Returns edge if it exists

    int MAX_EDGE_SEGMENT_LENGTH;                                     // Length of the longest segment in meters

private:
    typedef pair<int, float>                                             _pair;
    typedef priority_queue<_pair,
                           vector<_pair>,
                           function<bool(const _pair &, const _pair &)>> PQ;

    void update_ch_priorities(int v,
                              float a, float b,
                              PQ &pq,
                              vector<float> &priorities,
                              vector<int> &old_to_new_id,
                              bool initialize_priority);             // Update the pririties for Contraction Hierarchies

    KDTree                         kd_tree;                          // The KD Tree that contains node_ids
    vector<LatLng>                 coordinates;                      // The coordinates, indexed with node_ID
    vector<vector<AdjListElement>> adjacency_lists;                  // Contains the forward edges information
    int                            directed_edges_counter;           // Number of edges in a graph
    vector<int>                    node_to_connected_component_id;   // if con_comp[i] == con_comp[j] they are connected
    int                            current_component_id;             // Number of edges in a graph
};

#endif // GRAPH_STORAGE_H