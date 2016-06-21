#include "GraphStorage.h"

class QueryGraph
{
public:
    QueryGraph(const GraphStorage &G,
               const NearestEdgeInfo &info_1,
               const NearestEdgeInfo &info_2);              // Create the query graph
    ~QueryGraph();                                          // Destroy the query graph

    const LatLng         &GetCoordinate(int node_id) const; // Get the latitude,longitude of a node
    vector<AdjacentEdge> GetForwardEdges(int u) const;      // Forward adjacent edges of the vertex u
    vector<AdjacentEdge> GetBackwardEdges(int u) const;     // Backward adjacent edges of the vertex u

    int from, to;                                           // The from and to nodes of the query

private:
    struct VirtualEdge
    {
        int   u, v;
        float w;

        VirtualEdge(int u, int v, float w) :
            u(u),
            v(v),
            w(w)
        { }
    };

    const LatLng        p1, p2;
    const GraphStorage  &G;
    vector<VirtualEdge> virtual_edges;

    static const int virtual_node_1_placeholder = numeric_limits<int>::max();
    static const int virtual_node_2_placeholder = numeric_limits<int>::max() - 1;
};