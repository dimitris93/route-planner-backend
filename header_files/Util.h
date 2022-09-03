#ifndef UTIL_H
#define UTIL_H

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

	// Print vector of type T
	template<typename T> static void PrintVector(const vector<T>& vec)
	{
		if (vec.size() == 0)
		{
			cout << "Empty vector." << endl;
			return;
		}

		int i = 0;
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