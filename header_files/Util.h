#include <vector>
#include <string>

using namespace std;

class Util
{
public:
    static vector<string> Tokenize(const string &str,
                                   const string &delimiter = " "); // Returns a vector with the tokens
    static string         DoubleToString(const double &d,          // Converts double to string with the
                                         int decimals);            // specified number of decimal digits
    template<typename T>
    bool                  Validate(std::string const &num);        // Validates if string is a number
    template<typename T>
    static string         IntToHex(T i);                           // Converts number to hexadecimal
};