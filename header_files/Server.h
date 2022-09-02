//#include <websocketpp/config/asio_no_tls.hpp>
//#include <websocketpp/server.hpp>
//#include "GraphStorage.h"
//
//using namespace std;
//
//class Server
//{
//public:
//    Server(unsigned short PORT,
//           const GraphStorage &G);           // Start server on the specified port
//    ~Server();                  // Destroy the object
//
//private:
//    typedef websocketpp::server<websocketpp::config::asio> srv;
//    typedef websocketpp::connection_hdl                    connection_handler;
//    typedef srv::message_ptr                               message_ptr;
//    typedef chrono::high_resolution_clock                  clock;
//
//    void OnMessage(connection_handler conn_hdl,
//                   message_ptr msg_ptr);        // Is called when a message is received
//    void OnOpen(connection_handler conn_hdl);   // Is called when a client connects
//    void Error(const string &error_string,
//               connection_handler conn_hdl,
//               chrono::time_point<clock> t0);   // Sends an error message to client and prints the error to console
//
//    srv                server;
//    const GraphStorage &G; // reference to the graph that will be used to perform shortest path algorithm
//};
