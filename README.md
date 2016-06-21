## Overview

This is the core of our routing engine. This is the application that calculates the shortest path from A->B. We also accept queries with in-between nodes, for example A->B->C. This case is easy because we calculate 2 shortest paths, one from A to B and one from B to C and then we combine them.

We receive queries (sequence of coordinates) from the user, we then compute the shortest path and send the response (the shortest path as a sequence of coordinates) back to the user.

## Query and response format

The technology we use to send messages (communicate) between the server and the client is Websockets, since that technology is built-in Javascript. However, for our C++ application, that technology is not built-in. At least not in an easy cross-platform way for windows and linux. This is the reason why we use a (headers-only) framework called [Websocketpp](https://github.com/zaphoyd/websocketpp). This is atually the only framework we use for the server-side.

When the user performs certain actions in the website, for example adding/moving/deleting nodes, a query is sent to the `server-side`. The **query** format we follow right now is really simple, for example:

Format: `Latitude_A Longitude_A Latitude_B Longitude_B`<br>
Example: `38.0006436 23.7786841 38.0022161 23.7793064`

The message we send to the client as a **response** from the c++ application after the shortest path algorithm is computed, is the **nodes sequence of the shortest path**. The message the user receives has the same format as the query, only this time we have a lot more nodes, each 2 adjacent nodes basically represent an edge in the shortest path. For example here is a **response** from the server.

Example: `38.0006897 23.7785669 38.0007352 23.7785848 38.0009246 23.7786363 38.0012873 23.7786724 38.0013635 23.7793300 38.0017295 23.7791770 38.0019797 23.7790311 38.0022526 23.7792636`

# Shortest path procedure

## Finding nearest edges, from user's given coordinates

The first thing we need to do before we can calculate the shortest path from A->B is to find the nearest point of our graph for each of the nodes A and B. Intuitively this means that we want to **snap the user's given coordinates to actual edges on our graph**. By that I imply that the user might select coordinates that are not on edges and might even kilometers far from any edge of our graph. Since a car can obviously only move on edges (aka road segments), we want to find the nearest edge, as well as the projected point that we get when we project the user's given coordinate to the nearest edge (we want the points to be on the edge). We do this for each coordinates A and B.
 
To find the nearest edge for each of the user's given coordinates we make use a KD-Tree. The KD-tree is a data structure that can help us search for nodes (not edges yet) within a radius. In our application, we start searching for nodes within a 250 meter radius. If no nearby node is found, then we increase that range by some amount until we find one or more nodes. The starting value of 250 meters is not abstract. It is the longest edge length in our entire graph. You might want to take a look [here](https://github.com/outerpixels/routing-engine-graph-extractor/blob/master/README.md) for the graph parsing. We split the edges that are longer than 250 meters by adding more nodes and edges to the graph. Overall, we end up adding only a tiny amount (1-2%) compared to the size of the graph.

We keep performing  `radius-search` in our KD-Tree in wider and wider circles from the user's given coordinate, until we find a collection of one or more nodes in this manner. For each node `u` of that collection, we look in our graph's edges (more info about the graph in-memory storage below) and find every edge that starts or ends (our graph representation allows us to look into the graph backwards as well) in `u`. This will result in getting a `set` of edges. Because we search in a radius of 250 or more, and the longest edge of the entire graph is 250, it is guaranteed that the nearest edge, is included in that `set` on edges. Otherwise, if we searched in a radius less than 250, we would ran into [this problem](http://stackoverflow.com/questions/19892564/find-nearest-edge-in-graph). For each edge in that set, we then calculate the distance from point (user's given coordinate) the the edge. The nearest distance will give us the nearest edge. 

## Query graph

Then, we take the projection (always on edge) of the user's node to the nearest edge we just calculated. This way we will find a point on the nearest edge. That point will be added to what we call `query graph` as a `virtual node`. For example:

[!2](https://i.gyazo.com/f804092e638dd6884ef84ef926161993.png)

In the example above, we added 2 `virtual nodes` (black circles) and 8 `virtual directed edges` (red arrows). This way, our shortest path algorithm will actually consider both ways of the road, since it happened so that both of those ways were double-ways. The query graph, is the graph on which the shortest path is actually performed on. It is our original graph, with a few `virtual nodes` and `virtual directed edges` if necessary.
