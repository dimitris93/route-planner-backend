## Overview

This is the core of our routing engine. This is the application that calculates the shortest path from A->B. We also accept queries with in-between nodes, for example A->B->C. This case is easy because we calculate 2 shortest paths, one from A to B and one from B to C and then we combine them.

We receive queries (sequence of coordinates) from the user, we then compute the shortest path and send the response (the shortest path as a sequence of coordinates) back to the user.

## Query and response format

The technology we use to send messages (communicate) between the server and the client is Websockets, since that technology is built-in Javascript. However, for our C++ application, that technology is not built in. At least not in an easy cross-platform way for windows and linux. This is the reason why we use a headers-only framework called `[Websocketpp](https://github.com/zaphoyd/websocketpp)`. This is atually the only framework we use for the server-side.

When the user performs certain actions in the website, for example adding/moving/deleting nodes, a query is sent to the `server-side`. The **query** format we follow right now is really simple, for example:

Format: `Latitude_A Longitude_A Latitude_B Longitude_B`<br>
Example: `38.0006436 23.7786841 38.0022161 23.7793064`

The message we send to the client as a **response** from the c++ application after the shortest path algorithm is computed, is the **nodes sequence of the shortest path**. The message we receive is in the same format as the query we sent, only this time we have a lot more nodes, each 2 adjacent nodes basically represent an edge in the shortest path. For example here is a **response** from the server.

Example: `38.0006897 23.7785669 38.0007352 23.7785848 38.0009246 23.7786363 38.0012873 23.7786724 38.0013635 23.7793300 38.0017295 23.7791770 38.0019797 23.7790311 38.0022526 23.7792636`
