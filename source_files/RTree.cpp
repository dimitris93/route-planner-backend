#include "RTree.h"

#include <memory>
#include <utility>

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
	// Expand the chosen leaf's rectangle to include the new entry
	chosen_leaf.r = chosen_leaf.r.Expand(entry_r);

	// SplitNodes - Quadratic-cost Algorithm
	chosen_leaf.SplitNode(entry_r, *this);
}

template<typename T, typename T2>
void RTree::Node::DoSplitNode(vector<unique_ptr<T>>& pointers,
							  unique_ptr<T2>&        first_node,
							  unique_ptr<T2>&        second_node,
							  const Rectangle&       entry_r,
							  RTree&                 rtree)
{
	if (pointers.size() <= rtree.M)
	{
		cout << "The node's entries (or children) need to be > M to perform a split." << endl;
		exit(-1);
	}

	vector<unique_ptr<T>> group_1;
	vector<unique_ptr<T>> group_2;

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
			const Rectangle& r1 = pointers[i]->r;
			const Rectangle& r2 = pointers[j]->r;
			const double     d  = r1.Expand(r2).ComputeArea() - r1.ComputeArea() - r2.ComputeArea();
			if (d > max_d)
			{
				max_d        = d;
				seed_1_index = i;
				seed_2_index = j;
			}
		}
	}
	group_1.emplace_back(std::move(pointers[seed_1_index]));   // move seed 1 to group 1
	group_2.emplace_back(std::move(pointers[seed_2_index]));   // move seed 2 to group 2
	// Erase Null pointers
	Util::EraseVecIndices<unique_ptr<T>>(pointers, {seed_1_index, seed_2_index});

	// Initialize the rectangles for the groups
	Rectangle r_group_1 = group_1[0]->r;
	Rectangle r_group_2 = group_2[0]->r;

	// PickNext
	while (true)
	{
		// No more nodes left to choose from
		if (pointers.empty())
		{
			break;
		}

		// We need to ensure that the groups have a minimum of m nodes.
		if (Util::min(group_1.size(), group_2.size()) + pointers.size() == rtree.m)
		{
			auto& smallest_group = group_1.size() < group_2.size() ? group_1 : group_2;
			Util::MovePtr(pointers, smallest_group);
			break;
		}

		max_d               = 0;
		int  chosen_i       = 0;
		bool chosen_group_1 = true;
		for (int i = 0; i < pointers.size(); ++i)
		{
			const Rectangle& r_i      = pointers[i]->r;
			const double     r_i_area = r_i.ComputeArea();
			const double     d1       = r_group_1.Expand(r_i).ComputeArea() - r_i_area;
			const double     d2       = r_group_2.Expand(r_i).ComputeArea() - r_i_area;
			double           d        = fabs(d1 - d2);
			if (d > max_d)
			{
				max_d          = d;
				chosen_i       = i;
				chosen_group_1 = d1 < d2;
			}
		}
		auto& chosen_group      = chosen_group_1 ? group_1 : group_2;
		auto& chosen_group_rect = chosen_group_1 ? r_group_1 : r_group_2;

		// Expand the rectangle of the group
		chosen_group_rect = chosen_group_rect.Expand(pointers[chosen_i]->r);
		//  Move the chosen children (or entry) to the chosen group
		Util::MovePtr<T>(pointers,
						 chosen_i,
						 chosen_group);
	}

	// Now that we have group_1 and group_2, we need to update our r-tree accordingly.

	// Set first node's children (or entries) as group_1
	Util::MovePtr(group_1, first_node->pointers);
	first_node->r      = r_group_1;   // update rectangle
	first_node->parent = parent;      // update parent

	// Set second node's children (or entries) as group_2
	Util::MovePtr(group_2, second_node->pointers);
	second_node->r      = r_group_2;   // update rectangle
	second_node->parent = parent;      // update parent

	// Having no parent, means this node is the root.
	// At this point, we just split the root, so we need to make a new root
	if (parent == nullptr)
	{
		Rectangle _r = r.Expand(second_node->r);

		vector<unique_ptr<Node>> _children;
		_children.push_back(std::move(first_node));
		_children.push_back(std::move(second_node));

		shared_ptr<NonLeaf> _parent(nullptr);   // root has no parent

		shared_ptr<NonLeaf> new_root = make_shared<NonLeaf>(_children,   // new root has the 2 splits as children
															_r,
															_parent);
		rtree.root                   = new_root;

		for (const auto& child : _children)
		{
			child->parent = new_root;
		}
	}
	// If this node is not the root, attempt to split its parent node
	else
	{
		parent->SplitNode(entry_r, rtree);
	}
}

void RTree::Leaf::SplitNode(const Rectangle& entry_r,
							RTree&           rtree)
{
	// Adjust covering rectangle of parent, if not root
	if (parent != nullptr)
	{
		parent->r = parent->r.Expand(entry_r);
	}

	// If size <= M, then don't split node
	if (entries.size() <= rtree.M)
	{
		return;
	}

	unique_ptr<Leaf> first_node(new Leaf());
	unique_ptr<Leaf> second_node(new Leaf());

	// Split this node into two nodes.
	// "this" node becomes the first split and the newly created node becomes the second split.
	DoSplitNode(entries,
				first_node,    // first split
				second_node,   // second split
				entry_r,
				rtree);
}

void RTree::NonLeaf::SplitNode(const Rectangle& entry_r,
							   RTree&           rtree)
{
	//	// Adjust covering rectangle of parent
	//	parent->r = parent->r.Expand(entry_r);
	//
	//	if (children.size() <= rtree.M)
	//	{
	//		return;
	//	}
	//
	//	// Split this node into two nodes.
	//	// "this" node becomes the first split and the newly created node becomes the second split.
	//	DoSplitNode(children,
	//				std::make_unique<NonLeaf>(),   // this will be the second split of the node
	//				entry_r,
	//				rtree);
	//
	//	parent->SplitNode(entry_r, rtree);
}

RTree::Leaf& RTree::Leaf::ChooseLeaf(const Rectangle& entry_r)
{
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

RTree::Node::Node(RTree::Rectangle    _r,
				  shared_ptr<NonLeaf> _parent) :
	r(_r),
	parent(_parent)
{
}

RTree::NonLeaf::NonLeaf(vector<unique_ptr<Node>>& _children,
						const RTree::Rectangle&   _r,
						shared_ptr<NonLeaf>&      _parent) :
	Node(_r, _parent),
	children(std::move(_children))
{
}

RTree::Leaf::Leaf(vector<unique_ptr<IndexEntry>>& _entries,
				  const RTree::Rectangle&         _r,
				  shared_ptr<NonLeaf>&            _parent) :
	Node(_r, _parent),
	entries(std::move(_entries))
{
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
