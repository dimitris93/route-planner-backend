#include "Graph.h"

using namespace std;

Node::Node(GpsCoordinate coord,
		   subgraphid_t  subgraph_id) :
	coord(coord),
	subgraph_id(subgraph_id)
{
}

AdjacentEdge::AdjacentEdge(nodeid_t v,
						   float    w,
						   EdgeType edge_type,
						   bool     is_startpoint) :
	v(v),
	w(w),
	edge_type(edge_type),
	is_startpoint(is_startpoint)
{
}

Graph::Graph() :
	nodes(),
	adjacent_edges_vec()
{
}

size_t Graph::CountNodes() const
{
	return nodes.size();
}

size_t Graph::CountEdges() const
{
	return adjacent_edges_vec.size();
}

void Graph::AddNode(nodeid_t             node_id,
					const GpsCoordinate& coord)
{
	// Node ids must begin with 0 and increase by 1 at a time.
	if (node_id == nodes.size())
	{
		nodes.emplace_back(coord);
	}
	else
	{
		cout << "Node IDs must start from 0 and increase by 1, until N." << endl
			 << "You tried to add a node with id=" << node_id << " when id=" << nodes.size() << " was expected." << endl;
		exit(-1);
	}
}

void Graph::AddEdge(nodeid_t u,
					nodeid_t v,
					float    w,
					EdgeType edge_type,
					bool     is_startpoint)
{
	if (u >= nodes.size() ||
		v >= nodes.size())
	{
		cout << "Node IDs do not exist." << endl;
		exit(-1);
	}

	adjacent_edges_vec.resize(nodes.size());

	if (edge_type == EdgeType::TWO_WAY)
	{
		adjacent_edges_vec[u].emplace_back(v,
										   w,
										   edge_type,
										   is_startpoint);
		adjacent_edges_vec[v].emplace_back(u,
										   w,
										   edge_type,
										   is_startpoint);
	}
	else
	{
		auto const opposite_edge_type =
				edge_type == EdgeType::FORWARD ? EdgeType::BACKWARD : EdgeType::FORWARD;
		adjacent_edges_vec[u].emplace_back(v,
										   w,
										   edge_type,
										   is_startpoint);
		adjacent_edges_vec[v].emplace_back(u,
										   w,
										   opposite_edge_type,
										   is_startpoint);
	}
}

vector<AdjacentEdge> Graph::GetAdjacentEdges(nodeid_t node_id,
											 bool     backwards_graph) const
{
	vector<AdjacentEdge> ret;

	// Forward edges
	if (!backwards_graph)
	{
		for (const auto& e : adjacent_edges_vec[node_id])
		{
			if (e.edge_type != EdgeType::BACKWARD)
			{
				ret.emplace_back(e);
			}
		}
		return ret;
	}
	// Backward edges
	else
	{
		for (const auto& e : adjacent_edges_vec[node_id])
		{
			if (e.edge_type != EdgeType::FORWARD)
			{
				ret.emplace_back(e);
			}
		}
		return ret;
	}
}

QueryGraph::QueryGraph(Graph& G, GpsCoordinate a, GpsCoordinate b) :
	G(G),
	a(a),
	b(b)
{
}

vector<AdjacentEdge> QueryGraph::GetForwardEdges(nodeid_t node_id) const
{
	return {};
}
