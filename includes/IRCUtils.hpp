#ifndef FT_IRC_IRCUTILS_HPP
#define FT_IRC_IRCUTILS_HPP
#include <iostream>
#include <vector>

# ifndef DEBUG
#  define DEBUG false
# endif

/// Prints a debug message if DEBUG is true
#if DEBUG == true
#define debug(x) std::cerr << "[DEBUG] " << x << std::endl
#else
#define debug(x)
#endif

//  << operator override for std::vector<T>
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	os << "[";
	for (size_t i = 0; i < v.size(); i++) {
		os << v[i];
		if (i != v.size() - 1) {
			os << ", ";
		}
	}
	return os << "]";
}

#endif //FT_IRC_IRCUTILS_HPP
