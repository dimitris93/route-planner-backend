#ifndef UTIL_H
#define UTIL_H

typedef unsigned int nodeid_t;
typedef unsigned int subgraphid_t;

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Util
{
public:
	static vector<string> Tokenize(const string& str,
								   const string& delim = " ");   // Returns a vector with the tokens
	static string         DoubleToString(const double& d,        // Convert double as string, with target precision
										 int           precision = 15);    // 15 is the precision that is guaranteed by "double".
	static size_t         abs(size_t x);
	static size_t         min(size_t x1, size_t x2);

	template<typename T> static void EraseVecIndices(vector<T>&  vec,   // Erase indices from vector
													 vector<int> indices)
	{
		std::sort(indices.begin(), indices.end(), greater<>());
		for (size_t i = 0; i < indices.size(); ++i)
		{
			vec.erase(vec.begin() + i);
		}
	}
	template<typename T> static void MovePtr(vector<unique_ptr<T>>& vec_from,   // Move a pointer from one vector (of pointers), to another
											 size_t                 i,
											 vector<unique_ptr<T>>& vec_to)
	{
		vec_to.emplace_back(std::move(vec_from[i]));
		vec_from.erase(vec_from.begin() + i);
	}
	//	template<typename T> static void MovePtr(vector<unique_ptr<T>>& vec_from,   // Move pointers from one vector (of pointers), to another
	//											 vector<size_t>         indices,
	//											 vector<unique_ptr<T>>& vec_to)
	//	{
	//		std::sort(indices.begin(), indices.end(), greater<>());
	//		for (size_t i = 0; i < indices.size(); ++i)
	//		{
	//			vec_to.emplace_back(std::move(vec_from[i]));
	//			vec_from.erase(vec_from.begin() + i);
	//		}
	//	}
	template<typename T> static void MoveVecPtr(vector<unique_ptr<T>>& vec_from,   // Move all pointers from one vector, to another
												vector<unique_ptr<T>>& vec_to)
	{
		vec_to.reserve(vec_from.size() + vec_to.size());
		std::move(vec_from.begin(), vec_from.end(), std::back_inserter(vec_to));
		vec_from.erase(vec_from.begin(), vec_from.end());
	}

	// Print vector of type T
	template<typename T> static void PrintVector(const vector<T>& vec)
	{
		if (vec.size() == 0)
		{
			cout << "Empty vector." << endl;
			return;
		}

		size_t i = 0;
		cout << "[" << endl;
		for (auto& x : vec)
		{
			cout << "  " << i << ": " << x << endl;
			i++;
		}
		cout << "]" << endl;
	}
};

#endif   // UTIL_H