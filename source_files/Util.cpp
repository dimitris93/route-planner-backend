#include <iomanip>
#include <sstream>
#include "Util.h"

vector<string> Util::Tokenize(const string &str, const string &delimiter)
{
    vector<string>    tokens;
    // Skip delimiter at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiter, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiter, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiter.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiter, pos);
        // Find next "non-delimiter"
        pos     = str.find_first_of(delimiter, lastPos);
    }

    return tokens;
}

string Util::DoubleToString(const double &d, int decimals)
{
    std::ostringstream out;
    out << fixed << setprecision(decimals) << d;
    return out.str();
}

template<typename T>
bool Util::Validate(std::string const &num)
{
    T                 value;
    std::stringstream stream(num);
    stream >> value;

    // If the stream is already in the error state peak will not change it.
    // Otherwise stream should be good and there should be no more data
    // thus resulting in a peek returning an EOF
    return (stream) && stream.peek() == std::char_traits<typename std::stringstream::char_type>::eof();
}

template<typename T>
std::string Util::IntToHex(T i)
{
    std::stringstream stream;
    stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << i;
    return stream.str();
}