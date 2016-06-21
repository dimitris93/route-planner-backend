#include "KDTree.h"
#include <algorithm>
#include <iostream>

using namespace std;

KDTree::KDTree(const vector<LatLng> &coordinates) :
    coordinates(coordinates)
{

}

void KDTree::Create()
{
    // KD Tree that contains 0 elements
    if (coordinates.empty())
        return; // empty KD Tree

    // Initialize the kd_tree vector to the proper size and set the node values to -1
    kd_tree = vector<int>(pow(2, (int) (log(coordinates.size()) / log(2) + 1)) - 1, -1);
    // Create the vectors that will hold the node_id values of the sorted coordinates
    vector<int> sorted_indexes_xy(coordinates.size());
    vector<int> sorted_indexes_yx(coordinates.size());
    vector<int> temp_array(coordinates.size());
    // Fill vectors with numbers 0 to n
    std::iota(sorted_indexes_xy.begin(), sorted_indexes_xy.end(), 0);
    std::iota(sorted_indexes_yx.begin(), sorted_indexes_yx.end(), 0);
    // Sort vector by x and y
    std::sort(sorted_indexes_xy.begin(),
              sorted_indexes_xy.end(),
              [&](int i, int j)
              {
                  const int x1 = coordinates[i].lat_int;
                  const int x2 = coordinates[j].lat_int;
                  if (x1 < x2)
                      return true;
                  if (x1 > x2)
                      return false;
                  return coordinates[i].lng_int < coordinates[j].lng_int;
              });
    // Sort vector by y and x
    std::sort(sorted_indexes_yx.begin(),
              sorted_indexes_yx.end(),
              [&](int i, int j)
              {
                  const int y1 = coordinates[i].lng_int;
                  const int y2 = coordinates[j].lng_int;
                  if (y1 < y2)
                      return true;
                  if (y1 > y2)
                      return false;
                  return coordinates[i].lat_int < coordinates[j].lat_int;
              });

    // The root node
    CreateNode(sorted_indexes_xy, sorted_indexes_yx, temp_array,
               0,                      // first index
               coordinates.size() - 1, // last index
               0,                      // depth
               0);                     // the index of this node in our binary tree's vector
}

KDTree::~KDTree()
{

}

void KDTree::Print()
{
    if (kd_tree.empty())
    {
        cout << "Empty KD Tree." << "\n";
        return;
    }

    const int levels = log(kd_tree.size() + 1) / log(2);

    for (int level = 0; level < levels; ++level)
    {
        const int first = pow(2, level) - 1;
        const int last  = pow(2, level + 1) - 2;;

        cout << "Level " << level << ": ";
        for (int i = first; i <= last; ++i)
        {
            cout << kd_tree[i] << " ";
        }
        cout << "\n";
    }
}

void KDTree::CreateNode(vector<int> &sorted_indexes_xy,
                        vector<int> &sorted_indexes_yx,
                        vector<int> &temp_array,
                        int index_first,
                        int index_last,
                        int depth,
                        int index)
{
    const bool SPLIT_BY_X = depth % 2 == 0; // 0 is our X axis and 1 is our Y axis

    // This node is a leaf
    if (index_first == index_last)
    {
        if (SPLIT_BY_X)
            kd_tree[index] = sorted_indexes_xy[index_first]; // set this node's value
        else
            kd_tree[index] = sorted_indexes_yx[index_first]; // set this node's value
        return;
    }

    const int   median_index  = index_first + (index_last - index_first) / 2;
    int         current_index = index_first;
    vector<int> median_indexes; // the indexes of the coordinates that have x,y equal to x0,y0

    // Prepare our sorted_indexes_yx array for the next split
    if (SPLIT_BY_X)
    {
        kd_tree[index] = sorted_indexes_xy[median_index];   // set this node's value to the node_id of the median
        const int x0 = coordinates[kd_tree[index]].lat_int; // median x
        const int y0 = coordinates[kd_tree[index]].lng_int; // median y

        // For every index in the current region
        for (int i = index_first; i <= index_last; ++i)
        {
            const int xi = coordinates[sorted_indexes_yx[i]].lat_int;
            const int yi = coordinates[sorted_indexes_yx[i]].lng_int;

            // Coordinate (xi, yi) belongs to the left region
            if (xi < x0 || (xi == x0 && yi < y0))
            {
                temp_array[current_index] = sorted_indexes_yx[i];
                current_index++;
            }
            else if (xi == x0 && yi == y0) // median
            {
                median_indexes.push_back(i);
            }
        }
        // For every element equal to median
        for (int i = 0; i < median_indexes.size(); ++i)
        {
            temp_array[current_index] = median_indexes[i];
            current_index++;
        }
        // For every index in the current region
        for (int i = index_first; i <= index_last; ++i)
        {
            const int xi = coordinates[sorted_indexes_yx[i]].lat_int;
            const int yi = coordinates[sorted_indexes_yx[i]].lng_int;

            // Coordinate (xi, yi) belongs to the right region
            if (xi > x0 || (xi == x0 && yi > y0))
            {
                temp_array[current_index] = sorted_indexes_yx[i];
                current_index++;
            }
        }
    }
    else // Prepare our sorted_indexes_xy array for the next split
    {
        kd_tree[index] = sorted_indexes_yx[median_index];   // set this node's value to the node_id of the median
        const int x0 = coordinates[kd_tree[index]].lat_int; // median x
        const int y0 = coordinates[kd_tree[index]].lng_int; // median y

        // For every index in the current region
        for (int i = index_first; i <= index_last; ++i)
        {
            const int xi = coordinates[sorted_indexes_xy[i]].lat_int;
            const int yi = coordinates[sorted_indexes_xy[i]].lng_int;

            // Coordinate (xi, yi) belongs to the left region
            if (yi < y0 || (yi == y0 && xi < x0))
            {
                temp_array[current_index] = sorted_indexes_xy[i];
                current_index++;
            }
            else if (xi == x0 && yi == y0) // median
            {
                median_indexes.push_back(i);
            }
        }
        // For every element equal to median
        for (int i = 0; i < median_indexes.size(); ++i)
        {
            temp_array[current_index] = median_indexes[i];
            current_index++;
        }
        // For every index in the current region
        for (int i = index_first; i <= index_last; ++i)
        {
            const int xi = coordinates[sorted_indexes_xy[i]].lat_int;
            const int yi = coordinates[sorted_indexes_xy[i]].lng_int;

            // Coordinate (xi, yi) belongs to the right region
            if (yi > y0 || (yi == y0 && xi > x0))
            {
                temp_array[current_index] = sorted_indexes_xy[i];
                current_index++;
            }
        }
    }

    // Recycle arrays
    if (SPLIT_BY_X)
        std::swap(sorted_indexes_yx, temp_array);
    else
        std::swap(sorted_indexes_xy, temp_array);

    // If we have 2 elements, then the median will be the element stored
    // on this node, and the other element will be on the right node.
    // Which means that the left node will be empty.
    if (median_index != index_first)
        // Left child
        CreateNode(sorted_indexes_xy, sorted_indexes_yx, temp_array,
                   index_first,      // first index
                   median_index - 1, // last index
                   depth + 1,        // depth
                   2 * index + 1);   // left child index
    // Right child
    CreateNode(sorted_indexes_xy, sorted_indexes_yx, temp_array,
               median_index + 1, // first index
               index_last,       // last index
               depth + 1,        // depth
               2 * index + 2);   // right child index

    // Recycle arrays
    if (SPLIT_BY_X)
        std::swap(sorted_indexes_yx, temp_array);
    else
        std::swap(sorted_indexes_xy, temp_array);
}

int KDTree::NNSearch(const LatLng &target) const
{
    int          best_node  = -1;
    double       best_dist  = numeric_limits<double>::max();
    const double target_lat = target.getLat();
    const double target_lng = target.getLng();
    NNSearchRecursive(target_lat, target_lng,
                      0, best_node, best_dist);
    return best_node;
}

vector<int> KDTree::RadiusSearch(const LatLng &source, const double &radius) const
{
    vector<int>  nodes;
    const double source_lat = source.getLat();
    const double source_lng = source.getLng();
    RadiusSearchRecursive(source_lat, source_lng, radius, 0, nodes);
    return nodes;
}

void KDTree::NNSearchRecursive(const double &target_lat, const double &target_lng,
                               int current_index, int &best_node, double &best_dist) const
{
    int current_node;

    if (current_index >= kd_tree.size() || (current_node = kd_tree[current_index]) == -1)
        return;

    const LatLng &current_point   = coordinates[current_node];
    const double current_lat      = current_point.getLat();
    const double current_lng      = current_point.getLng();
    const double current_distance = LatLng::DistanceInMeters(current_lat, current_lng, target_lat, target_lng);

    // If the current location is better than the best known location,
    // update the best known location
    if (current_distance < best_dist)
    {
        best_dist = current_distance;
        best_node = current_node;
    }

    const int  depth      = log(current_index + 1) / log(2);
    const bool SPLIT_BY_X = depth % 2 == 0;

    // Recursively search the half of the tree that contains the target point
    bool search_left;
    if (SPLIT_BY_X)
    {
        if (target_lat < current_lat || (target_lat == current_lat && target_lng < current_lng))
        {
            search_left = true;
            NNSearchRecursive(target_lat, target_lng, 2 * current_index + 1, best_node, best_dist); // search left
        }
        else
        {
            search_left = false;
            NNSearchRecursive(target_lat, target_lng, 2 * current_index + 2, best_node, best_dist); // search right
        }
    }
    else
    {
        if (target_lng < current_lng || (target_lng == current_lng && target_lat < current_lat))
        {
            search_left = true;
            NNSearchRecursive(target_lat, target_lng, 2 * current_index + 1, best_node, best_dist); // search left
        }
        else
        {
            search_left = false;
            NNSearchRecursive(target_lat, target_lng, 2 * current_index + 2, best_node, best_dist); // search right
        }
    }

    // If the candidate hypersphere crosses this splitting plane, look on the
    // other side of the plane by examining the other subtree
    double dist;
    if (SPLIT_BY_X)
        dist = LatLng::DistanceInMeters(current_lat, target_lng, target_lat, target_lng);
    else
        dist = LatLng::DistanceInMeters(target_lat, current_lng, target_lat, target_lng);

    if (dist < best_dist)
    {
        // If we previously searched left, then search right, otherwise search left
        if (search_left)
            NNSearchRecursive(target_lat, target_lng, 2 * current_index + 2, best_node, best_dist); // search right
        else
            NNSearchRecursive(target_lat, target_lng, 2 * current_index + 1, best_node, best_dist); // search left
    }

    return;
}

void KDTree::RadiusSearchRecursive(const double &source_lat, const double &source_lng, const double &radius,
                                   int current_index, vector<int> &nodes) const
{
    int current_node;

    if (current_index >= kd_tree.size() || (current_node = kd_tree[current_index]) == -1)
        return;

    const LatLng &current_point   = coordinates[current_node];
    const double current_lat      = current_point.getLat();
    const double current_lng      = current_point.getLng();
    const double current_distance = LatLng::DistanceInMeters(current_lat, current_lng, source_lat, source_lng);

    // If the current location is inside the circle
    if (current_distance <= radius)
    {
        nodes.push_back(current_node); // add it
    }

    const int depth      = log(current_index + 1) / log(2);
    const int SPLIT_BY_X = depth % 2 == 0;

    const double dist = SPLIT_BY_X ?
                        LatLng::DistanceInMeters(current_lat, source_lng, source_lat, source_lng) :
                        LatLng::DistanceInMeters(source_lat, current_lng, source_lat, source_lng);

    if (dist > radius)
    {
        if (SPLIT_BY_X)
        {
            if (source_lat < current_lat)
                RadiusSearchRecursive(source_lat, source_lng, radius, 2 * current_index + 1, nodes); // search left
            else
                RadiusSearchRecursive(source_lat, source_lng, radius, 2 * current_index + 2, nodes); // search right
        }
        else
        {
            if (source_lng < current_lng)
                RadiusSearchRecursive(source_lat, source_lng, radius, 2 * current_index + 1, nodes); // search left
            else
                RadiusSearchRecursive(source_lat, source_lng, radius, 2 * current_index + 2, nodes); // search right
        }
    }
    else
    {
        RadiusSearchRecursive(source_lat, source_lng, radius, 2 * current_index + 1, nodes); // search left
        RadiusSearchRecursive(source_lat, source_lng, radius, 2 * current_index + 2, nodes); // search right
    }
}