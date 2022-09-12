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
	class NonLeaf;
	class Node
	{
	public:
		Node()          = default;
		virtual ~Node() = default;
		Node(Rectangle           _r,
			 shared_ptr<NonLeaf> _parent);
		virtual Leaf& ChooseLeaf(const Rectangle& entry_r) = 0;   // pure virtual
		virtual void  SplitNode(const Rectangle& entry_r,
								RTree&           rtree)              = 0;   // pure virtual

		Rectangle           r{};
		shared_ptr<NonLeaf> parent;

	protected:
		template<typename T, typename T2>
		void DoSplitNode(vector<unique_ptr<T>>& pointers,
						 unique_ptr<T2>&        first_node,
						 unique_ptr<T2>&        second_node,
						 const Rectangle&       entry_r,
						 RTree&                 rtree);   // helper function
	};

	class NonLeaf : public Node
	{
	public:
		NonLeaf() = default;
		NonLeaf(vector<unique_ptr<Node>>& _children,
				const RTree::Rectangle&   _r,
				shared_ptr<NonLeaf>&      _parent);
		Leaf& ChooseLeaf(const Rectangle& entry_r) override;
		void  SplitNode(const Rectangle& entry_r,
						RTree&           rtree) override;

		vector<unique_ptr<Node>>  children;
		vector<unique_ptr<Node>>& pointers = children;
	};

	class Leaf : public Node
	{
	public:
		Leaf()            = default;
		Leaf(const Leaf&) = default;
		Leaf(vector<unique_ptr<IndexEntry>>& _entries,
			 const RTree::Rectangle&         _r,
			 shared_ptr<NonLeaf>&            _parent);
		Leaf& ChooseLeaf(const Rectangle& entry_r) override;
		void  SplitNode(const Rectangle& entry_r,
						RTree&           rtree) override;

		vector<unique_ptr<IndexEntry>>  entries;
		vector<unique_ptr<IndexEntry>>& pointers = entries;
	};

	const int m = 4;   // m <= M/2
	const int M = 8;

	shared_ptr<Node> root;
};

#endif   // RTREE_H
