#ifndef SERVER_HPP
#define SERVER_HPP
# ifndef IRC_MAX_CONNECTIONS
#  define IRC_MAX_CONNECTIONS 420
# endif
# ifndef RECV_BUFFER_SIZE
#  define RECV_BUFFER_SIZE 1024
# endif

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


class Server;
class Client;
#include "IRCUtils.hpp"
#include "cmd/CmdInterface.hpp"
#include "Client.hpp"

typedef struct sockaddr SocketAddr;
typedef struct sockaddr_in SocketAddrIn;
typedef struct pollfd PollFd;

typedef std::map<int, Client*> ClientsMap;
typedef std::map<const std::string, CmdInterface*> ServerCommandsMap;
typedef std::vector<PollFd> AllPollFdsVector;

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
	/// Map of currently connected clients (key: client fd, value: client object)
	ClientsMap clients;
	/// Map of implemented commands (key: command name, value: command object)
	ServerCommandsMap commands;

	/// Socket file descriptor
	int socketFd;
	/// Socket address
	SocketAddrIn socketAddr;
	/// polling fds
	AllPollFdsVector allPollFds;

// Server-only methods
private:
	void initCommands();
	void startListening();

	/// Accept connection: add new user
	void acceptConnection();

	/// Parse data received from client
	void receiveClientMessage(Client* client);

	/// Try to parse the client buffer as a command if the buffer contains a complete command (ends with '\r\n')
	bool tryToRunClientCommand(Client* client);

	/// Delete disconnected clients
	void deleteDisconnectedClients();
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
