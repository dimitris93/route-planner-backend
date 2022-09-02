#ifndef GRAPH_H
#define GRAPH_H

#include "LatLng.h"

using namespace std;

enum EdgeType
{
	FORWARD_EDGE,
	BACKWARD_EDGE,
	DOUBLE_WAY_EDGE
};

class Node
{
public:
	Node(LatLng       latlng,
		 unsigned int subgraph_id = 0) :
		latlng(latlng),
		subgraph_id(subgraph_id)
	{
	}

private:
	LatLng         latlng;
	unsigned short subgraph_id;
};

class AdjacentEdge
{
public:
	AdjacentEdge(unsigned int v,
				 float        w,
				 EdgeType     edge_type,
				 bool         is_startpoint) :
		v(v),
		w(w),
		edge_type(edge_type),
		is_startpoint(is_startpoint)
	{
	}

	unsigned v;
	float    w;
	char     edge_type;
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

	size_t CountNodes() const;   // Number of nodes
	size_t CountEdges() const;   // Number of directed edges
	void   AddNode(unsigned int  node_id,
				   const LatLng& latlng);   // Add node
	void   AddEdge(unsigned int u,
				   unsigned int v,
				   float        w,
				   EdgeType     edge_type,
				   bool         is_startpoint);   // Add edge (u,v) with w weight

	//	const LatLng&        GetCoordinate(int node_id) const;   // Get the latitude,longitude of a node
	//	NearestEdgeInfo      GetNearestEdge(const LatLng& source,
	//										const double& radius) const;   // Nearest edge from coordinate
	//	vector<AdjacentEdge> GetForwardEdges(int u) const;                 // Forward adjacent edges of the vertex u
	//	vector<AdjacentEdge> GetBackwardEdges(int u) const;                // Backward adjacent edges of the vertex u
	//	Edge                 FindForwardEdge(int u, int v) const;          // Returns edge if it exists
	//	bool                 AreConnected(int u, int v) const;             // Returns edge if it exists

	vector<AdjacentEdge> GetForwardEdges(unsigned int node_id) const;

private:
	vector<Node>                 nodes;                // The nodes, indexed with node_ID
	vector<vector<AdjacentEdge>> adjacent_edges_vec;   // Vector where the X-element contains all edges connected to node with id == X
};

#endif   // GRAPH_H
