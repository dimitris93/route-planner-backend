#include "GpsCoordinate.h"
#include "Util.h"

int main(int argc, char* argv[])
{
	//	cout << LatLng::ClosestPointToLineSegment(LatLng(4, 7),
	//											  LatLng(2, 3),
	//											  LatLng(6, 5))
	//					.ToString()
	//		 << endl;

	//		Graph G;
	//		G.AddNode(0, GpsCoordinate(4, 7));
	//		G.AddNode(1, GpsCoordinate(2, 3));
	//		G.AddNode(2, GpsCoordinate(6, 5));
	//		G.AddNode(3, GpsCoordinate(6, 5));
	//		G.AddNode(4, GpsCoordinate(6, 5));
	//		G.AddEdge(0, 1, 6, EdgeType::TWO_WAY, true);
	//		G.AddEdge(1, 2, 5, EdgeType::TWO_WAY, true);
	//		G.AddEdge(2, 3, 5, EdgeType::TWO_WAY, true);
	//		G.AddEdge(3, 4, 1, EdgeType::TWO_WAY, true);
	//		G.AddEdge(4, 0, 1, EdgeType::TWO_WAY, true);
	//		G.AddEdge(1, 3, 2, EdgeType::TWO_WAY, true);
	//		G.AddEdge(1, 4, 2, EdgeType::TWO_WAY, true);
	//
	//		//	cout << G.CountNodes() << endl;
	//		//	cout << G.CountEdges() << endl;
	//
	//		Route r(RoutePlanner::plan_route(0, 2, G, RoutePlanner::Dijkstra));
	//		Util::PrintVector<unsigned int>(r.nodeid_sequence);
	//		cout << r.sum_weight << endl;
	//
	//		cout << "-----------------" << endl;
	//
	//		Route r2(RoutePlanner::plan_route(0, 2, G, RoutePlanner::Bidirectional_Dijkstra));
	//		Util::PrintVector<unsigned int>(r2.nodeid_sequence);
	//		cout << r2.sum_weight << endl;

	//	RTree::Rectangle r1(GpsCoordinate(1, 3),
	//						GpsCoordinate(2, 5));
	//	RTree::Rectangle r2(GpsCoordinate(2, 6),
	//						GpsCoordinate(3, 7));
	//	auto             r3 = r1.Expand(r2);
	//	cout << r3.p1.ToString() << endl;
	//	cout << r3.p2.ToString() << endl;
	//
	//	cout << r3.ComputePerimeter() << endl;

	vector<unique_ptr<GpsCoordinate>> gps_ptrs;
	vector<unique_ptr<GpsCoordinate>> gps_ptrs_2;
	gps_ptrs.emplace_back(new GpsCoordinate(1, 3));
	gps_ptrs.emplace_back(new GpsCoordinate(12, 32));
	gps_ptrs.emplace_back(new GpsCoordinate(16, 52));
	gps_ptrs_2.emplace_back(new GpsCoordinate(5, 7));
	gps_ptrs_2.emplace_back(new GpsCoordinate(3, 79));

	cout << gps_ptrs.size() << endl;
	cout << gps_ptrs_2.size() << endl;


	cout << gps_ptrs.size() << endl;
	cout << gps_ptrs_2.size() << endl;



	for (int i = 0; i < 5; ++i)
	{
		for (int j = i + 1; j < 5; ++j)
		{
			cout << i << " " << j << endl;
		}
	}


	//	RTree rtree;
	//	rtree.AddEdge(0,
	//				  1,
	//				  GpsCoordinate(1, 3),
	//				  GpsCoordinate(2, 5));
}

// int main(int argc, char* argv[])
// int f()
//{
//	chrono::time_point<chrono::high_resolution_clock> t0 = chrono::high_resolution_clock::now();
//
//	// Initialize the file stream and get the number of vertices and edges of the graph
//	ifstream nodes_file("data/gr_nodes.txt");
//	ifstream ways_file("data/gr_ways.txt");
//	string   line;
//
//	// Create a GraphStorage with the specified number of vertices and no edges
//	GraphStorage G(4801053 + 1, 250);
//
//	// Add nodes to the graph
//	getline(nodes_file, line);          // first row is a description of the file's format
//	while (getline(nodes_file, line))   // each line contains the (node_id, lat, lng) values
//	{
//		vector<string> tokens = Util::Tokenize(line);
//		try
//		{
//			int    node_id = stoi(tokens[0]);
//			double lat     = stod(tokens[1]);
//			double lng     = stod(tokens[2]);
//			G.SetCoordinate(node_id, lat, lng);
//		}
//		catch (exception& e)
//		{
//			cout << tokens[0] << " " << tokens[1] << " " << tokens[2] << "\n";
//			return 0;
//		}
//	}
//	nodes_file.close();
//
//	// Create KD Tree
//	G.CreateKDTree();
//
//	// Add edges to the graph
//	getline(ways_file, line);          // first row is a description of the file's format
//	while (getline(ways_file, line))   // each line contains the (u, v, w) values
//	{
//		vector<string> tokens = Util::Tokenize(line);
//		try
//		{
//			const int    a                 = stoi(tokens[0]);
//			const int    b                 = stoi(tokens[1]);
//			const string forward_mode      = tokens[2];
//			const string backward_mode     = tokens[3];
//			const float  forward_duration  = stof(tokens[4]);
//			const float  backward_duration = stof(tokens[5]);
//			const float  duration          = tokens[6] == "None" ? 0 : stof(tokens[6]);
//			const bool   is_startpoint     = tokens[7] == "True";
//			const bool   double_way        = forward_mode != "INACCESSIBLE" &&
//									backward_mode != "INACCESSIBLE" &&
//									(duration > 0 || forward_duration == backward_duration);
//
//			if (double_way)
//			{
//				if (duration != 0)   // ferry
//					G.AddEdge(a, b, duration, true, is_startpoint);
//				else
//					G.AddEdge(a, b, forward_duration, true, is_startpoint);
//			}
//			else
//			{
//				if (duration != 0)   // ferry
//				{
//					if (forward_mode != "INACCESSIBLE")
//						G.AddEdge(a, b, duration, false, is_startpoint);
//					if (forward_mode != "INACCESSIBLE")
//						G.AddEdge(b, a, duration, false, is_startpoint);
//				}
//				else
//				{
//					if (forward_mode != "INACCESSIBLE")
//						G.AddEdge(a, b, forward_duration, false, is_startpoint);
//					if (backward_mode != "INACCESSIBLE")
//						G.AddEdge(b, a, backward_duration, false, is_startpoint);
//				}
//			}
//		}
//		catch (exception& e)
//		{
//			cout << line << "\n";
//			return 0;
//		}
//	}
//	ways_file.close();
//
//	//    // Add contraction hierarchies
//	//    G.ComputeContractionHierarchies();
//	//    cout << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t0).count()
//	//    << " ms elapsed.\n";
//
//	t0 = chrono::high_resolution_clock::now();
//	// Find connected components
//	G.FindConnectedComponents();
//	cout << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t0).count()
//		 << " ms elapsed.\n";
//
//	// Start server on the specified port
//	Server(9090, G);
//
//	return 0;
// }
