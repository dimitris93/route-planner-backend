#include "QueryGraph.h"

/*
 * Shortest algorithm for computing the shortest path tree from
 * a starting vertex s. We assume that all weights are non-negative.
 * An index priority queue is being used which is implemented
 * using a binary heap tree and a collection that can give us the
 * underlying position of an element in the priority queue
 * so that we can increase its priority if needed.
 */
class ShortestPath
{
public:
    enum Algorithm
    {
        Dijkstra,
        Bidirectional_Dijkstra
    };

    ShortestPath(int s, int ignore_node,
                 float  uv_cost,
                 vector<AdjacentEdge> &targets,
                 const vector<vector<AdjListElement>> &adjacency_lists);
    ShortestPath(const QueryGraph &queryG,
                 vector<int> &shortest_path,
                 float &path_cost,
                 Algorithm algorithm);                     // Runs the algorithm to find the s->t shortest path
    ~ShortestPath();                                       // Destroys the shortest path tree information

private:
    void SimpleDijkstra(const QueryGraph &queryG,
                        vector<int> &shortest_path,
                        float &path_cost);                 // Performs simple dijkstra
    void BidirectionalDijkstra(const QueryGraph &queryG,
                               vector<int> &shortest_path,
                               float &path_cost);          // Performs bidirectional dijkstra
};