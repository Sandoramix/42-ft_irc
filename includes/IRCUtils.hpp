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
#include <poll.h>
#include <algorithm>
#include <iostream>
#include <vector>

#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

typedef int SocketFd;
typedef struct sockaddr SocketAddr;
typedef struct sockaddr_in SocketAddrIn;
typedef struct pollfd PollFd;
typedef std::vector<PollFd> AllPollFdsVector;


# define RESET "\033[0m"
# define RED "\033[31m"
# define MAGENTA "\033[35m"
# define YELLOW "\033[33m"
# define GREEN "\033[32m"
# define CYAN "\033[36m"
# define BLUE "\033[34m"
# define GREY "\033[90m"
# define BOLD "\033[1m"

/// Prints a debug message if DEBUG is true
#if DEBUG==true
# define debug(x) std::cerr << GREY << "[DEBUG]\t\t" << x << RESET << std::endl
# define debugResponse(x) std::cerr << BLUE BOLD << "[RESPONSE]\t" << x << RESET << std::endl
# define debugError(x) std::cerr << MAGENTA << "[DEBUG-ERROR]\t" << x << RESET << std::endl
# define debugWarning(x) std::cerr << YELLOW << "[DEBUG-WARNING]\t" << x << RESET << std::endl
# define debugSuccess(x) std::cerr << GREEN << "[DEBUG-SUCCESS]\t" << x << RESET << std::endl
# define debugInfo(x) std::cerr << CYAN << "[DEBUG-INFO]\t" << x << RESET << std::endl
#else
#define debug(x)
#define debugResponse(x)
#define debugError(x)
#define debugWarning(x)
#define debugSuccess(x)
#define debugInfo(x)
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
