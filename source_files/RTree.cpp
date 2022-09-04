#include "RTree.h"

RTree::RTree()
{
//	RTree::Rectangle r1(GpsCoordinate(1, 3),
//						GpsCoordinate(2, 5));
//	RTree::Rectangle r2(GpsCoordinate(2, 6),
//						GpsCoordinate(3, 7));
//	auto             r3 = r1.Expand(r2);
//	cout << r3.p1.ToString() << endl;
//	cout << r3.p2.ToString() << endl;
//
//	cout << r3.ComputePerimeter() << endl;


}

void RTree::AddIndexEntry(IndexEntry E)
{
//	if (root->r.ComputePerimeter() == 0)
//	{
//		Leaf leaf;
//		root    = &leaf;
//		Leaf& L = ChooseLeaf(leaf, E);
//	}
}

RTree::Leaf& RTree::Leaf::ChooseLeaf(RTree::IndexEntry E)
{
	return *this;
}

RTree::Leaf& RTree::NonLeaf::ChooseLeaf(RTree::IndexEntry E)
{
	size_t i             = 0;
	size_t min_i         = 0;
	double min_perimeter = 0;
	for (; i < children.size(); ++i)
	{
		double perimeter = children[i]->r.Expand(E.r).ComputePerimeter();
		if (perimeter < min_perimeter)
		{
			min_i         = i;
			min_perimeter = perimeter;
		}
	}

	return children[min_i]->ChooseLeaf(E);
}

RTree::Node::Node() :
	r(GpsCoordinate(0, 0),
	  GpsCoordinate(0, 0))
{
}

RTree::Leaf::Leaf() :
	Node(),
	entries()
{
}



// ---------------------- Rectangle ----------------------
RTree::Rectangle::Rectangle(GpsCoordinate p1, GpsCoordinate p2) :
	p1(p1),
	p2(p2)
{
}

RTree::Rectangle RTree::Rectangle::Expand(RTree::Rectangle r2) const
{
	return {GpsCoordinate(min(p1.GetLat(), r2.p2.GetLat()),
						  min(p1.GetLng(), r2.p2.GetLng())),
			GpsCoordinate(max(p1.GetLat(), r2.p2.GetLat()),
						  max(p1.GetLng(), r2.p2.GetLng()))};
}

double RTree::Rectangle::ComputePerimeter() const
{
	const double W = p2.GetLat() - p1.GetLat();
	const double L = p2.GetLng() - p1.GetLng();
	return 2 * (W + L);
}
