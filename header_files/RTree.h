#ifndef RTREE_H
#define RTREE_H

#include "GpsCoordinate.h"

using namespace std;

class RTree
{
public:
	RTree();
	void AddEdge(nodeid_t u, nodeid_t v,
				 GpsCoordinate p1, GpsCoordinate p2);

private:
	class LineSegment
	{
	public:
		LineSegment(nodeid_t a, nodeid_t b);

		nodeid_t a;
		nodeid_t b;
	};

	class Rectangle
	{
	public:
		Rectangle() = default;
		Rectangle(GpsCoordinate p1, GpsCoordinate p2);   // Rectangle that bounds edge a---b

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
		virtual Leaf& ChooseLeaf(const Rectangle& entry_r) = 0;   // pure virtual
		virtual void  SplitNode(const RTree::Rectangle& entry_r,
								const RTree&            rtree)        = 0;   // pure virtual

		Rectangle        r;
		unique_ptr<Node> parent;

	protected:
		template<typename T>
		void DoSplitNode(vector<unique_ptr<T>>&  children_or_entries,
						 const RTree::Rectangle& entry_r,
						 const RTree&            rtree);   // helper function
	};

	class NonLeaf : public Node
	{
	public:
		NonLeaf() = default;
		Leaf& ChooseLeaf(const Rectangle& entry_r) override;
		void  SplitNode(const RTree::Rectangle& entry_r,
						const RTree&            rtree) override;

		vector<unique_ptr<Node>> children;
	};

	class Leaf : public Node
	{
	public:
		Leaf() = default;
		Leaf& ChooseLeaf(const Rectangle& entry_r) override;
		void  SplitNode(const RTree::Rectangle& entry_r,
						const RTree&            rtree) override;

		vector<unique_ptr<IndexEntry>> entries;
	};

	const int m = 4;   // m <= M/2
	const int M = 8;

	unique_ptr<Node> root;
};

#endif   // RTREE_H
