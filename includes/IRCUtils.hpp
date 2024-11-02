#ifndef FT_IRC_IRCUTILS_HPP
#define FT_IRC_IRCUTILS_HPP
# ifndef DEBUG
#  define DEBUG false
# endif
#include <vector>
#include <exception>
#include <iostream>
#include <string>
#include <map>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <algorithm>
#include <iostream>
#include <vector>

typedef int SocketFd;
typedef struct sockaddr SocketAddr;
typedef struct sockaddr_in SocketAddrIn;
typedef struct pollfd PollFd;
typedef std::vector<PollFd> AllPollFdsVector;

/// Prints a debug message if DEBUG is true
#if DEBUG==true
#define debug(x) std::cerr << "[DEBUG] " << x << std::endl
#else
#define debug(x)
#endif

//  << operator override for std::vector<T>
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	os << "[";
	for (size_t i = 0; i<v.size(); i++) {
		if (i > 0) { os << ", "; }
		os << v[i];
	}
	return os << "]";
}

#endif //FT_IRC_IRCUTILS_HPP
