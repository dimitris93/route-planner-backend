#ifndef SHORTESTPATH_H
#define SHORTESTPATH_H

#include "Graph.h"

/*
 * Shortest algorithm for computing the shortest path tree from
 * a starting vertex s. We assume that all weights are non-negative.
 * An index priority queue is being used which is implemented
 * using a binary heap tree and a collection that can give us the
 * underlying position of an element in the priority queue
 * so that we can increase its priority if needed.
 */

class Route
{
public:
	Route(vector<unsigned int> nodeid_sequence,
		  float                cost) :
		nodeid_sequence(std::move(nodeid_sequence)),
		cost(cost)
	{
	}

	vector<unsigned int> nodeid_sequence;   // as a sequence of coordinate ids
	float                cost;              // cost of the route
};

class RoutePlanner
{
public:
	enum Algorithm
	{
		Dijkstra,
		Bidirectional_Dijkstra
	};

	static Route plan_route(unsigned int s,
							unsigned int t,
							const Graph& G,
							Algorithm    algorithm);   // Compute the shortest path from point A to B
};

#endif   // SHORTESTPATH_H