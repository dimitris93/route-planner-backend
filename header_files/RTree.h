#ifndef RTREE_H
#define RTREE_H

#include "GpsCoordinate.h"

using namespace std;

class RTree
{
public:
	RTree();

private:
	class LineSegment
	{
		LineSegment();

		unsigned int a;   // node ID a
		unsigned int b;   // node ID b
	};

	class Rectangle
	{
	public:
		Rectangle(GpsCoordinate p1,
				  GpsCoordinate p2);   // Rectangle that bounds edge a---b

		GpsCoordinate p1;
		GpsCoordinate p2;

		Rectangle Expand(Rectangle r2) const;
		double    ComputePerimeter() const;
	};

	class IndexEntry
	{
	public:
		IndexEntry();

		Rectangle   r;
		LineSegment l;
	};

	class Leaf;
	class Node
	{
	public:
		Node();

		virtual Leaf& ChooseLeaf(IndexEntry E) = 0;
		Rectangle     r;
	};

	class Leaf : public Node
	{
	public:
		Leaf();

		vector<IndexEntry> entries;
		Leaf&              ChooseLeaf(IndexEntry E);
	};

	class NonLeaf : public Node
	{
	public:
		NonLeaf();

		vector<unique_ptr<Node>> children;
		Leaf&                    ChooseLeaf(IndexEntry E);
	};

	const unsigned int m = 4;   // m <= M/2
	const unsigned int M = 8;

	unique_ptr<Node> root;

	void AddIndexEntry(IndexEntry E);
};

#endif   // RTREE_H
