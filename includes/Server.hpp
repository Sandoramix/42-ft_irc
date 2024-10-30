#ifndef SERVER_HPP
#define SERVER_HPP

#include <exception>
#include <map>
#include <string>
#include <netinet/in.h>

class Server;
#include "cmd/CmdInterface.hpp"

extern bool SERVER_RUNNING;

typedef struct sockaddr_in SocketAddr;
typedef std::map<const std::string&, CmdInterface*> ServerCommandsMap;

class Server {

// Assignable from outside
private:
	unsigned short port;
	std::string password;
	std::string host;

// Server-only members
private:
	/// Map of implemented commands (key: command name, value: command object)
	ServerCommandsMap allCommands;

	/// Socket file descriptor
//	int socketFd;
	/// Socket address
//	SocketAddr socketAddr;


// Server-only methods
private:
	void initCommands();
	void initSocket();

public:
	Server(const std::string& host, const std::string& port, const std::string& password);
	~Server();
	void run();

	class BadConfigException : public std::exception {
	private:
		std::string msg;
	public:
		explicit BadConfigException(const std::string& msg);
		virtual ~BadConfigException() throw();
		BadConfigException(const BadConfigException&);
		virtual const char* what() const throw();
	};
};

#endif //SERVER_HPP
