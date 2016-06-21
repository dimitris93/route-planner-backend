#ifndef KD_TREE_H
#define KD_TREE_H

#include <vector>
#include "LatLng.h"

using namespace std;

class KDTree
{
public:
    KDTree(const vector<LatLng> &coordinates);            // Create an empty KD Tree
    ~KDTree();                                            // Destroy the KD Tree

    void        Create();                                 // Create a balanced KD Tree from a set of coordinates
    void        Print();                                  // Print each level of the KD Tree
    int         NNSearch(const LatLng &target) const;     // Begin the recursive Nearest Neighbor Search
    vector<int> RadiusSearch(const LatLng &source,
                             const double &radius) const; // Begin the recursive search for nodes within a radius

private:
    vector<int>          kd_tree;                         // The KD Tree is a binary tree stored as a vector
    const vector<LatLng> &coordinates;                    // Reference to the coordinates vector

    void CreateNode(vector<int> &sorted_indexes_xy,       // The node ids sorted by x (lat) and y (lng)
                    vector<int> &sorted_indexes_yx,       // The node ids sorted by y (lng) and x (lat)
                    vector<int> &temp_array,              // A temporary array
                    int index_first,                      // The first index of the first coordinate of this region
                    int index_last,                       // The last index of the first coordinate of this region
                    int depth,                            // The depth of this node compared to the root
                    int index);                           // The index of this node in our binary tree's vector
    void NNSearchRecursive(const double &target_lat,
                           const double &target_lng,
                           int current_index,
                           int &best_node,
                           double &best_dist) const;      // Recursive Nearest Neighbor Search
    void RadiusSearchRecursive(const double &source_lat,
                               const double &source_lng,
                               const double &radius,
                               int current_index,
                               vector<int> &nodes) const; // Recursively search for nodes within a radius
};

#endif // KD_TREE_H