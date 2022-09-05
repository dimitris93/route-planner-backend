#ifndef GRAPH_H
#define GRAPH_H

#include "GpsCoordinate.h"

using namespace std;

enum EdgeType
{
	FORWARD,
	BACKWARD,
	TWO_WAY   // two-way edges need to have the same w on both sides
};

class Node
{
public:
	Node(GpsCoordinate coord,
		 subgraphid_t  subgraph_id = 0);   // we want nodes to have subgraph_ids so that we know if any given 2 nodes are connected

	GpsCoordinate  coord;
	unsigned short subgraph_id;
};

class AdjacentEdge
{
public:
	AdjacentEdge(nodeid_t v,
				 float    w,
				 EdgeType edge_type,
				 bool     is_startpoint);

	nodeid_t v;
	float    w;
	EdgeType edge_type;
	bool     is_startpoint;
};

/*
 * Graph (with directed edges and weights) implementation using Adjacency Lists.
 * Each adjacency list is implemented as a std::vector,
 * and they are all stored in an outer std::vector.
 * We use the node ID as index to retrieve the adjacency list of that node.
 */
class Graph
{
public:
	Graph();   // Create GraphStorage and allocate memory

	size_t               CountNodes() const;   // Number of nodes
	size_t               CountEdges() const;   // Number of directed edges
	void                 AddNode(nodeid_t             node_id,
								 const GpsCoordinate& coord);   // Add node
	void                 AddEdge(nodeid_t u,
								 nodeid_t v,
								 float    w,
								 EdgeType edge_type,
								 bool     is_startpoint);   // Add edge (u,v) with w weight
	vector<AdjacentEdge> GetAdjacentEdges(nodeid_t node_id,
										  bool     backwards_graph = false) const;

private:
	vector<Node>                 nodes;                // The nodes, indexed with node_ID
	vector<vector<AdjacentEdge>> adjacent_edges_vec;   // Vector where the X-element contains all edges connected to node with id == X
};

class QueryGraph
{
public:
	QueryGraph(Graph& G, GpsCoordinate a, GpsCoordinate b);   // Create GraphStorage and allocate memory
	vector<AdjacentEdge> GetForwardEdges(nodeid_t node_id) const;

private:
	const Graph&                 G;
	GpsCoordinate                a;
	GpsCoordinate                b;
	vector<Node>                 virtual_nodes;                // virtual nodes
	vector<vector<AdjacentEdge>> virtual_adjacent_edges_vec;   // virtual edges
};

#endif   // GRAPH_H
