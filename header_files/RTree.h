#ifndef RTREE_H
#define RTREE_H

#include "GpsCoordinate.h"

using namespace std;

class RTree
{
public:
	RTree();
	void AddEdge(int u, int v, GpsCoordinate p1, GpsCoordinate p2);

private:
	class LineSegment
	{
	public:
		LineSegment(int a, int b);

		unsigned int a;   // node ID a
		unsigned int b;   // node ID b
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
		virtual Leaf& ChooseLeaf(IndexEntry E) = 0;

		Rectangle r;
	};

	class Leaf : public Node
	{
	public:
		Leaf();
		Leaf& ChooseLeaf(IndexEntry E);

		vector<unique_ptr<IndexEntry>> entries;
	};

	class NonLeaf : public Node
	{
	public:
		NonLeaf();
		Leaf& ChooseLeaf(IndexEntry E);

		vector<unique_ptr<Node>> children;
	};

	const unsigned int m = 4;   // m <= M/2
	const unsigned int M = 8;

	unique_ptr<Node> root;
};

#endif   // RTREE_H
