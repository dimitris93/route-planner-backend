#ifndef ROUTEPLANNER_H
#define ROUTEPLANNER_H

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
	Route();
	Route(vector<nodeid_t> nodeid_sequence,
		  float            cost);

	vector<nodeid_t> nodeid_sequence;   // A sequence of coordinate ids with the shortest path
	float            sum_weight;        // Total cost of the route. The sum weight of all edges.
										// Expressed in the same units as the graph's edge weights.
};

class RoutePlanner
{
public:
	enum Algorithm
	{
		Dijkstra,
		Bidirectional_Dijkstra
	};

	static Route plan_route(nodeid_t     s,
							nodeid_t     t,
							const Graph& G,
							Algorithm    algorithm);   // Compute the shortest path from point A to B
};

#endif   // ROUTEPLANNER_H