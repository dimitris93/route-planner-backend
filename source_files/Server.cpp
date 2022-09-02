//#include "Server.h"
//#include "ShortestPath.h"
//#include "Util.h"
//
//Server::Server(unsigned short PORT, const GraphStorage &G) :
//    G(G)
//{
//    // Disable log messages
//    server.set_access_channels(websocketpp::log::alevel::none);
//    server.set_error_channels(websocketpp::log::elevel::none);
//    // Set event handlers
//    server.set_message_handler(bind(&Server::OnMessage, this, placeholders::_1, placeholders::_2));
//    server.set_open_handler(bind(&Server::OnOpen, this, placeholders::_1));
//    server.init_asio();
//    server.listen(PORT);
//    // Start accepting connections from clients
//    server.start_accept();
//    cout << "==== Server starting on PORT: " << PORT << " ====\n";
//    server.run();
//}
//
//Server::~Server()
//{
//
//}
//
//void Server::OnMessage(connection_handler conn_hdl, message_ptr msg_ptr)
//{
//    // Get the time from clock
//    chrono::time_point<clock> t0 = clock::now();
//
//    const string msg = msg_ptr->get_payload();
//    // Print message
//    cout << "==== Message received ====\n" << msg << "\n";
//
//    vector<string> tokens = Util::Tokenize(msg);
//
//    // If message is not formatted correctly
//    if (tokens.size() < 4 || tokens.size() % 2 != 0)
//    {
//        Error("Error: Message was not properly formatted.", conn_hdl, t0);
//        return;
//    }
//
//    // Get the coordinates of the query and validate them
//    vector<LatLng> coordinates(tokens.size() / 2);
//    for (int       i      = 0; i < tokens.size(); i += 2)
//    {
//        try
//        {
//            const double lat = stod(tokens[i]);
//            const double lng = stod(tokens[i + 1]);
//
//            // If (lat, lng) is not a valid coordinate in the real world
//            if (!LatLng::Validate(lat, lng))
//            {
//                Error("Error: Invalid latitude and/or longitude values.", conn_hdl, t0);
//                return;
//            }
//            coordinates[i / 2] = LatLng(lat, lng);
//        }
//        catch (exception &e)
//        {
//            Error("Error from converting the user's given coordinates (string) to double: ", conn_hdl, t0);
//            return;
//        }
//    }
//
//    // For every coordinate of the query do a look up in
//    // our KD Tree to find the nearest edge
//    // This vector stores information about the nearest edge to each coordinate
//    vector<NearestEdgeInfo> nearest_edges_info;
//    nearest_edges_info.reserve(coordinates.size());
//    // For each coordinate that the user's query contains
//    for (int i       = 0; i < coordinates.size(); ++i)
//    {
//        // Get nearest edge
//        NearestEdgeInfo e = G.GetNearestEdge(coordinates[i], G.MAX_EDGE_SEGMENT_LENGTH);
//        for (int        k = 1; !e.edge_found and k < 100; ++k)
//        {
//            e = G.GetNearestEdge(coordinates[i], G.MAX_EDGE_SEGMENT_LENGTH * (k * 2));
//        }
//
//        if (i > 0 && !G.AreConnected(nearest_edges_info[i-1].edge.u, e.edge.u))
//        {
//            Error("Error: One or more of your given coordinates were isolated (possibly on islands).", conn_hdl, t0);
//            return;
//        }
//
//        if (!e.edge_found)
//        {
//            Error("Error: One or more of your given coordinates were not near roads.", conn_hdl, t0);
//            return;
//        }
//        nearest_edges_info.push_back(e);
//    }
//
//
//    // Compute the shortest paths and fill up a vector with the coordinates of the
//    // total shortest path route.
//    // We might execute more than 1 shortest path
//    // algorithms because we accept A->B shortest path queries that allow
//    // the user to specify in-between nodes that he wants to visit.
//    vector<LatLng> path_coordinates;
//    for (int       i = 0; i < nearest_edges_info.size() - 1; ++i)
//    {
//        QueryGraph queryG(G,
//                          nearest_edges_info[i],
//                          nearest_edges_info[i + 1]);
//
//        // Run the shortest path algorithm
//        vector<int>  sp;
//        float        cost;
//        ShortestPath d(queryG, sp, cost, ShortestPath::Bidirectional_Dijkstra);
//
//        // If there is no A->B path
//        if (sp.empty())
//        {
//            Error("Error: Path doesn't exist.", conn_hdl, t0);
//            return;
//        }
//
//        // Avoid adding duplicate nodes.
//        // In this query A--->B--->C, node B will be added only once
//        if (i > 0)
//            path_coordinates.pop_back();
//
//        // Get path's coordinates
//        path_coordinates.reserve(sp.size());
//        for (int j = 0; j < sp.size(); ++j)
//            path_coordinates.push_back(queryG.GetCoordinate(sp[j]));
//    }
//
//    // We will send the path as a string of coordinates to the client
//    string   path_string;
//    // For every coordinate
//    for (int i       = 0; i < path_coordinates.size(); ++i)
//        path_string += path_coordinates[i].ToString() + " ";
//    path_string.pop_back(); // remove last space
//
//
//    // Send the shortest path as a response to the client
//    server.send(conn_hdl, path_string, websocketpp::frame::opcode::TEXT);
//
//    // Print time elapsed
//    cout << chrono::duration_cast<chrono::milliseconds>(clock::now() - t0).count() << " ms elapsed.\n";
//}
//
//void Server::OnOpen(connection_handler conn_hdl)
//{
//    cout << "==== Client Connected ====\n";
//}
//
//void Server::Error(const string &error_string, connection_handler conn_hdl, chrono::time_point<clock> t0)
//{
//    // Print error message to console
//    cout << error_string << "\n";
//    // Send error message to client
//    server.send(conn_hdl, error_string, websocketpp::frame::opcode::TEXT);
//    // Print time elapsed
//    cout << chrono::duration_cast<chrono::milliseconds>(clock::now() - t0).count() << " ms elapsed.\n";
//}