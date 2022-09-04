#include "RTree.h"

RTree::RTree() :
	root(new Leaf())
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
	root;
}

void RTree::AddEdge(int u, int v, GpsCoordinate p1, GpsCoordinate p2)
{

	IndexEntry E = IndexEntry(Rectangle(p1, p2),
							  LineSegment(u, v));

	Leaf& chosen_leaf = root->ChooseLeaf(E);

	chosen_leaf.entries.emplace_back(&E);

	// Split nodes
	vector<unique_ptr<IndexEntry>> split_1;
	vector<unique_ptr<IndexEntry>> split_2;
	Rectangle                      r_split_1;
	Rectangle                      r_split_2;
	if (chosen_leaf.entries.size() > M)
	{
		// PickSeeds
		// Select i and j with highest computed d, to be the first node in each split
		double max_d = 0;
		int    max_i = -1;
		int    max_j = -1;
		for (int i = 0; i < M + 1; ++i)
		{
			for (int j = i + 1; j < M + 1; ++j)
			{
				const Rectangle& r1 = chosen_leaf.entries[i]->r;
				const Rectangle& r2 = chosen_leaf.entries[j]->r;
				const double     d  = r1.Expand(r2).ComputeArea() - r1.ComputeArea() - r2.ComputeArea();
				if (d > max_d)
				{
					max_d = d;
					max_i = i;
					max_j = j;
				}
			}
		}
		r_split_1 = r_split_1.Expand(chosen_leaf.entries[max_i]->r);
		r_split_2 = r_split_2.Expand(chosen_leaf.entries[max_j]->r);
		split_1.emplace_back(std::move(chosen_leaf.entries[max_i]));
		split_2.emplace_back(std::move(chosen_leaf.entries[max_j]));
		chosen_leaf.entries.erase(chosen_leaf.entries.begin() + max_i);
		chosen_leaf.entries.erase(chosen_leaf.entries.begin() + max_j);

		// PickNext
		while (true)
		{
			// If no more nodes left, quit
			if (chosen_leaf.entries.size() == 0)
			{
				break;
			}
			// If there are just enough nodes to meet minimum requirements, then we only have 1 option
			if (Util::size_t_abs(split_1.size() - split_2.size()) <= chosen_leaf.entries.size())
			{
				auto & smallest_split = split_1.size() < split_2.size() ? split_1 : split_2;

				for (int i = 0; i < chosen_leaf.entries.size(); ++i)
				{
					smallest_split.emplace_back(std::move(chosen_leaf.entries[i]));
					chosen_leaf.entries.erase(chosen_leaf.entries.begin() + i);
				}

			}

			double max_d = 0;
			int    max_i = 0;
			bool   is_d1 = true;
			for (int i = 0; i < chosen_leaf.entries.size(); ++i)
			{
				const Rectangle& r      = chosen_leaf.entries[i]->r;
				const double     r_area = r.ComputeArea();
				const double     d1     = r_split_1.Expand(r).ComputeArea() - r_area;
				const double     d2     = r_split_2.Expand(r).ComputeArea() - r_area;
				double           d      = fabs(d1 - d2);
				if (d > max_d)
				{
					max_i = i;
					is_d1 = d1 < d2;
				}
			}
			if (is_d1)
			{
				split_1.emplace_back(std::move(chosen_leaf.entries[max_i]));
				chosen_leaf.entries.erase(chosen_leaf.entries.begin() + max_i);
			}
			else
			{
				split_2.emplace_back(std::move(chosen_leaf.entries[max_j]));
				chosen_leaf.entries.erase(chosen_leaf.entries.begin() + max_j);
			}
		}
	}
}

RTree::Leaf& RTree::Leaf::ChooseLeaf(RTree::IndexEntry E)
{
	return *this;
}

RTree::Leaf& RTree::NonLeaf::ChooseLeaf(RTree::IndexEntry E)
{
	size_t i        = 0;
	size_t min_i    = 0;
	double min_area = 0;
	for (; i < children.size(); ++i)
	{
		double area = children[i]->r.Expand(E.r).ComputeArea();
		if (area < min_area)
		{
			min_i    = i;
			min_area = area;
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

RTree::NonLeaf::NonLeaf() :
	Node(),
	children()
{
}

RTree::LineSegment::LineSegment(int a, int b) :
	a(a),
	b(b)
{
}

RTree::IndexEntry::IndexEntry(RTree::Rectangle   r,
							  RTree::LineSegment l) :
	r(r),
	l(l)
{
}

// ---------------------- Rectangle ----------------------
RTree::Rectangle::Rectangle() :
	p1(GpsCoordinate(0, 0)),
	p2(GpsCoordinate(0, 0))
{
}

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

double RTree::Rectangle::ComputeArea() const
{
	const double W = p2.GetLat() - p1.GetLat();
	const double L = p2.GetLng() - p1.GetLng();
	return W * L;
}
