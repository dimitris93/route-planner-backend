#include "RTree.h"

RTree::RTree() :
	root(new Leaf())
{
}

void RTree::AddEdge(nodeid_t u, nodeid_t v, GpsCoordinate p1, GpsCoordinate p2)
{
	Rectangle entry_r(p1, p2);

	// ChooseLeaf
	Leaf& chosen_leaf = root->ChooseLeaf(entry_r);

	// Add a new Index Entry to the chosen leaf
	chosen_leaf.entries.emplace_back(new IndexEntry(Rectangle(p1, p2),
													LineSegment(u, v)));

	// SplitNodes - Quadratic-cost Algorithm
	chosen_leaf.SplitNode(entry_r, *this);
}

template<typename T>
void RTree::Node::DoSplitNode(vector<unique_ptr<T>>&  children_or_entries,
							  const RTree::Rectangle& entry_r,
							  const RTree&            rtree)
{
	// If the chosen leaf's size becomes M + 1 after the insertion, then we need to split it.
	vector<unique_ptr<T>> group_1;
	vector<unique_ptr<T>> group_2;
	if (children_or_entries.size() > rtree.M)
	{
		// PickSeeds
		// Select the two nodes that would waste the most area, if they were put in the same group.
		// Put those 2 nodes in different groups.
		double max_d        = 0;
		int    seed_1_index = -1;
		int    seed_2_index = -1;
		for (int i = 0; i < rtree.M + 1; ++i)
		{
			for (int j = i + 1; j < rtree.M + 1; ++j)
			{
				const Rectangle& r1 = children_or_entries[i]->r;
				const Rectangle& r2 = children_or_entries[j]->r;
				const double     d  = r1.Expand(r2).ComputeArea() - r1.ComputeArea() - r2.ComputeArea();
				if (d > max_d)
				{
					max_d        = d;
					seed_1_index = i;
					seed_2_index = j;
				}
			}
		}
		group_1.emplace_back(std::move(children_or_entries[seed_1_index]));   // move seed 1 to group 1
		group_2.emplace_back(std::move(children_or_entries[seed_2_index]));   // move seed 2 to group 2
		// Erase Null pointers
		Util::EraseVecIndices<unique_ptr<T>>(children_or_entries, {seed_1_index, seed_2_index});

		// Initialize the rectangles for the groups
		Rectangle r_group_1 = group_1[0]->r;
		Rectangle r_group_2 = group_2[0]->r;

		// PickNext
		while (true)
		{
			// No more nodes left to choose from
			if (children_or_entries.empty())
			{
				break;
			}
			// We need to ensure that the groups have a minimum of m nodes.
			if (Util::min(group_1.size(), group_2.size()) + children_or_entries.size() == rtree.m)
			{
				auto& smallest_group = group_1.size() < group_2.size() ? group_1 : group_2;
				Util::MoveVecPtr(children_or_entries, smallest_group);
				break;
			}

			max_d               = 0;
			int  chosen_i       = 0;
			bool chosen_group_1 = true;
			for (int i = 0; i < children_or_entries.size(); ++i)
			{
				const Rectangle& rec    = children_or_entries[i]->r;
				const double     r_area = rec.ComputeArea();
				const double     d1     = r_group_1.Expand(rec).ComputeArea() - r_area;
				const double     d2     = r_group_2.Expand(rec).ComputeArea() - r_area;
				double           d      = fabs(d1 - d2);
				if (d > max_d)
				{
					max_d          = d;
					chosen_i       = i;
					chosen_group_1 = d1 < d2;
				}
			}
			Util::MovePtr<T>(children_or_entries,
							 chosen_i,
							 chosen_group_1 ? group_1 : group_2);
		}
	}

	parent->SplitNode();
}

void RTree::Leaf::SplitNode(const RTree::Rectangle& entry_r, const RTree& rtree)
{
	DoSplitNode(this->entries, entry_r, rtree);
}

void RTree::NonLeaf::SplitNode(const RTree::Rectangle& entry_r, const RTree& rtree)
{
	DoSplitNode(this->children, entry_r, rtree);
}

RTree::Leaf& RTree::Leaf::ChooseLeaf(const Rectangle& entry_r)
{
	parent->ChooseLeaf(Rectangle());
	return *this;
}

RTree::Leaf& RTree::NonLeaf::ChooseLeaf(const Rectangle& entry_r)
{
	int    min_i    = -1;
	double min_area = numeric_limits<double>::max();
	for (int i = 0; i < children.size(); ++i)
	{
		double area = children[i]->r.Expand(entry_r).ComputeArea();
		if (area < min_area)
		{
			min_i    = i;
			min_area = area;
		}
	}

	return children[min_i]->ChooseLeaf(entry_r);
}

// RTree::Leaf::Leaf() :
//	Node(),
//	entries()
//{
// }

RTree::LineSegment::LineSegment(nodeid_t a, nodeid_t b) :
	a(a),
	b(b)
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

RTree::IndexEntry::IndexEntry(RTree::Rectangle   r,
							  RTree::LineSegment l) :
	r(r),
	l(l)
{
}
