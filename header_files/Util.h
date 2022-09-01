#ifndef UTIL_H
#define UTIL_H

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
																 //	template<typename T> static void PrintVector(const vector<T>& vec);     // Prints vector
	static string DoubleToString(const double& num,
								 const int&    decimals = 7);   // specified number of decimal digits

	// Print vector of type T
	template<typename T> static void PrintVector(const vector<T>& vec)
	{
		if (vec.size() == 0)
			return;

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