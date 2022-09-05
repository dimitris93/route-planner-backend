#include "Util.h"

// vector<string> Util::Tokenize(const string& str, const string& delim)
//{
//	vector<string> tokens;
//
//	string s(str);
//	size_t pos = 0;
//	while ((pos = s.find(delim)) != string::npos)
//	{
//		tokens.push_back(s.substr(0, pos));
//		s.erase(0, pos + delim.length());
//	}
//	tokens.push_back(s);
//
//	return tokens;
// }

string Util::DoubleToString(const double& d, int precision)
{
	ostringstream ss;
	ss << std::setprecision(precision) << d;
	return ss.str();
}

size_t Util::abs(size_t x)
{
	return x > 0 ? x : -x;
}

size_t Util::min(size_t x1, size_t x2)
{
	return x1 < x2 ? x1 : x2;
}