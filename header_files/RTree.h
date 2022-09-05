#ifndef RTREE_H
#define RTREE_H

#include "GpsCoordinate.h"

using namespace std;

class RTree
{
public:
	RTree();
	void AddEdge(nodeid_t u, nodeid_t v, GpsCoordinate p1, GpsCoordinate p2);

private:
	class LineSegment
	{
	public:
		LineSegment(nodeid_t a, nodeid_t b);

		nodeid_t a;   // node ID a
		nodeid_t b;   // node ID b
	};

	class Rectangle
	{
	public:
		Rectangle();
		Rectangle(GpsCoordinate p1,
				  GpsCoordinate p2);   // Rectangle that bounds edge a---b

		GpsCoordinate p1;
		GpsCoordinate p2;

		Rectangle Expand(Rectangle r2) const;
		double    ComputeArea() const;
	};

	class IndexEntry
	{
	public:
		IndexEntry(Rectangle   r,
				   LineSegment l);

		Rectangle   r;
		LineSegment l;
	};

	class Leaf;
	class Node
	{
	public:
		Node();
		virtual Leaf& ChooseLeaf(const Rectangle& entry_r) = 0;   // this makes the Node class Abstract

		Rectangle r;
	};

	class Leaf : public Node
	{
	public:
		Leaf();
		Leaf& ChooseLeaf(const Rectangle& entry_r) override;

		vector<unique_ptr<IndexEntry>> entries;
	};

	class NonLeaf : public Node
	{
	public:
		NonLeaf();
		Leaf& ChooseLeaf(const Rectangle& entry_r) override;

		vector<unique_ptr<Node>> children;
	};

	const nodeid_t m = 4;   // m <= M/2
	const nodeid_t M = 8;

	unique_ptr<Node> root;
};

#endif   // RTREE_H
