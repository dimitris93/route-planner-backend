//#include "QueryGraph.h"
//
//QueryGraph::QueryGraph(const GraphStorage &G, const NearestEdgeInfo &info_1, const NearestEdgeInfo &info_2) :
//    G(G),
//    from(QueryGraph::virtual_node_1_placeholder),
//    to(QueryGraph::virtual_node_2_placeholder),
//    p1(info_1.projected_point),
//    p2(info_2.projected_point)
//{
//    const auto  &e1            = info_1.edge;
//    const auto  &e2            = info_2.edge;
//    const int   A              = e1.u;
//    const int   B              = e1.v;
//    const int   A2             = e2.u;
//    const int   B2             = e2.v;
//    double      clamped_ratio  = info_1.clamped_ratio;
//    double      clamped_ratio2 = info_2.clamped_ratio;
//    const float w              = e1.w;
//    const float w2             = e2.w;
//
//    const bool equal_edge = A == A2 && B == B2;
//    const bool same_edge  = A == B2 && B == A2;
//
//    // If edge A-B equals to A2-B2
//    if (equal_edge || same_edge)
//    {
//        // If A-B and A2-B2 are double ways
//        if (e1.type == Edge::DOUBLE_WAY_EDGE)
//        {
//            if (same_edge)
//                clamped_ratio2 = 1 - clamped_ratio2;
//
//            // A == from == to  <---> B
//            if (clamped_ratio == clamped_ratio2)
//            {
//                virtual_edges.push_back(VirtualEdge(from, to, 0));
//            }
//                // A<--->from<--->to<--->B
//            else if (clamped_ratio < clamped_ratio2)
//            {
//                virtual_edges.reserve(6);
//                virtual_edges.push_back(VirtualEdge(A, from, clamped_ratio * w));
//                virtual_edges.push_back(VirtualEdge(from, A, virtual_edges[0].w));
//                virtual_edges.push_back(VirtualEdge(from, to, (clamped_ratio2 - clamped_ratio) * w));
//                virtual_edges.push_back(VirtualEdge(to, from, virtual_edges[2].w));
//                virtual_edges.push_back(VirtualEdge(to, B, (1 - clamped_ratio2) * w));
//                virtual_edges.push_back(VirtualEdge(B, to, virtual_edges[4].w));
//            }
//            else // A<--->to<--->from<--->B
//            {
//                virtual_edges.reserve(6);
//                virtual_edges.push_back(VirtualEdge(A, to, clamped_ratio2 * w));
//                virtual_edges.push_back(VirtualEdge(to, A, virtual_edges[0].w));
//                virtual_edges.push_back(VirtualEdge(to, from, (clamped_ratio - clamped_ratio2) * w));
//                virtual_edges.push_back(VirtualEdge(from, to, virtual_edges[2].w));
//                virtual_edges.push_back(VirtualEdge(from, B, (1 - clamped_ratio) * w));
//                virtual_edges.push_back(VirtualEdge(B, from, virtual_edges[4].w));
//            }
//        }
//        else
//        {
//            Edge e3 = G.FindForwardEdge(B, A);
//            // Has edge B->A
//            if (e3.type != Edge::NULL_EDGE)
//            {
//                if (same_edge)
//                    clamped_ratio2 = 1 - clamped_ratio2;
//
//                // A == from == to  <---> B
//                if (clamped_ratio == clamped_ratio2)
//                {
//                    virtual_edges.push_back(VirtualEdge(from, to, 0));
//                }
//                    // A<--->from<--->to<--->B
//                else if (clamped_ratio < clamped_ratio2)
//                {
//                    virtual_edges.reserve(6);
//                    virtual_edges.push_back(VirtualEdge(A, from, clamped_ratio * w));
//                    virtual_edges.push_back(VirtualEdge(from, A, clamped_ratio * e3.w));
//                    virtual_edges.push_back(VirtualEdge(from, to, (clamped_ratio2 - clamped_ratio) * w));
//                    virtual_edges.push_back(VirtualEdge(to, from, (clamped_ratio2 - clamped_ratio) * e3.w));
//                    virtual_edges.push_back(VirtualEdge(to, B, (1 - clamped_ratio2) * w));
//                    virtual_edges.push_back(VirtualEdge(B, to, (1 - clamped_ratio2) * e3.w));
//                }
//                else // A<--->to<--->from<--->B
//                {
//                    virtual_edges.reserve(6);
//                    virtual_edges.push_back(VirtualEdge(A, to, clamped_ratio2 * w));
//                    virtual_edges.push_back(VirtualEdge(to, A, clamped_ratio2 * e3.w));
//                    virtual_edges.push_back(VirtualEdge(to, from, (clamped_ratio - clamped_ratio2) * w));
//                    virtual_edges.push_back(VirtualEdge(from, to, (clamped_ratio - clamped_ratio2) * e3.w));
//                    virtual_edges.push_back(VirtualEdge(from, B, (1 - clamped_ratio) * w));
//                    virtual_edges.push_back(VirtualEdge(B, from, (1 - clamped_ratio) * e3.w));
//                }
//            }
//            else // One way
//            {
//                // A == from == to ---> B
//                if (clamped_ratio == clamped_ratio2)
//                {
//                    virtual_edges.push_back(VirtualEdge(from, to, 0));
//                }
//                    // A---from--->to---B
//                else if (clamped_ratio < clamped_ratio2)
//                {
//                    virtual_edges.push_back(VirtualEdge(from, to, (clamped_ratio2 - clamped_ratio) * w));
//                }
//                else // A--->to---from--->B
//                {
//                    virtual_edges.reserve(2);
//                    virtual_edges.push_back(VirtualEdge(from, B, (1 - clamped_ratio) * w));
//                    virtual_edges.push_back(VirtualEdge(A, to, clamped_ratio2 * w));
//                }
//            }
//        }
//    }
//    else // e1,e2 are different
//    {
//        virtual_edges.push_back(VirtualEdge(from, B, (1 - clamped_ratio) * w));
//        if (e1.type == Edge::DOUBLE_WAY_EDGE)
//        {
//            virtual_edges.push_back(VirtualEdge(from, A, clamped_ratio * w));
//        }
//        else
//        {
//            // If graph has edge B->A
//            Edge e3 = G.FindForwardEdge(B, A);
//            if (e3.type != Edge::NULL_EDGE)
//            {
//                virtual_edges.push_back(VirtualEdge(from, A, clamped_ratio * e3.w));
//            }
//        }
//
//        virtual_edges.push_back(VirtualEdge(A2, to, clamped_ratio2 * w2));
//        if (e2.type == Edge::DOUBLE_WAY_EDGE)
//        {
//            virtual_edges.push_back(VirtualEdge(B2, to, (1 - clamped_ratio2) * w2));
//        }
//        else
//        {
//            // If graph has edge B2->A2
//            Edge e3 = G.FindForwardEdge(B2, A2);
//            if (e3.type != Edge::NULL_EDGE)
//            {
//                virtual_edges.push_back(VirtualEdge(B2, to, (1 - clamped_ratio2) * e3.w));
//            }
//        }
//    }
//}
//
//QueryGraph::~QueryGraph()
//{
//
//}
//
//vector<AdjacentEdge> QueryGraph::GetForwardEdges(int u) const
//{
//    vector<AdjacentEdge> list;
//    // If node is not virtual, look in our graph
//    if (u < G.GetNodes() && u >= 0)
//        list = G.GetForwardEdges(u);
//    for (const auto &e : virtual_edges)
//    {
//        if (u == e.u) // starts with u
//            list.push_back(AdjacentEdge(e.v, e.w));
//    }
//    return list;
//}
//
//vector<AdjacentEdge> QueryGraph::GetBackwardEdges(int u) const
//{
//    vector<AdjacentEdge> list;
//    // If node is not virtual, look in our graph
//    if (u < G.GetNodes() && u >= 0)
//        list = G.GetBackwardEdges(u);
//    for (const auto &e : virtual_edges)
//    {
//        if (u == e.v) // ends in u
//            list.push_back(AdjacentEdge(e.u, e.w));
//    }
//    return list;
//}
//
//const LatLng &QueryGraph::GetCoordinate(int node_id) const
//{
//    // If node is not virtual, look in our graph
//    if (node_id < G.GetNodes() && node_id >= 0)
//        return G.GetCoordinate(node_id);
//    else if (node_id == from)
//        return p1;
//    else if (node_id == to)
//        return p2;
//    else // this should never be called
//    {
//        cout << "Error: You requested a coordinate from a node_id that doesn't exist.\n";
//        return G.GetCoordinate(0);
//    }
//}
//
//
//
//
//
//
//
//
//
