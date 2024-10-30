#ifndef SERVER_HPP
#define SERVER_HPP
# ifndef IRC_MAX_CONNECTIONS
#  define IRC_MAX_CONNECTIONS 10
# endif

#include <exception>
#include <map>
#include <string>
#include <netinet/in.h>

class Server;
#include "cmd/CmdInterface.hpp"

typedef struct sockaddr_in SocketAddr;
typedef std::map<const std::string&, CmdInterface*> ServerCommandsMap;

extern bool SERVER_RUNNING;

class Server {
// Assignable from outside
private:
	unsigned short port;
	std::string password;
	std::string host;
	unsigned int maxConnections;

// Server-only members
private:
	/// Map of implemented commands (key: command name, value: command object)
	ServerCommandsMap allCommands;

	/// Socket file descriptor
	int socketFd;
	/// Socket address
	SocketAddr socketAddr;

// Server-only methods
private:
	void initCommands();
	void startListening();

public:
	Server(const std::string& host, const std::string& port, const std::string& password);
	~Server();
	void run();

//EXCEPTIONS
public:
	/// BAD CONFIGURATION EXCEPTION
	class BadConfigException : public std::exception {
	private:
		std::string msg;
	public:
		explicit BadConfigException(const std::string& msg);
		virtual ~BadConfigException() throw();
		virtual const char* what() const throw();
	};

	/// SERVER GENERIC EXCEPTION
	class ServerException : public std::exception {
	private:
		std::string msg;
	public:
		explicit ServerException(const std::string& msg);
		virtual ~ServerException() throw();
		virtual const char* what() const throw();
	};
};

#endif //SERVER_HPP
