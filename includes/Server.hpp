#ifndef SERVER_HPP
#define SERVER_HPP
# ifndef IRC_MAX_CONNECTIONS
#  define IRC_MAX_CONNECTIONS 420
# endif
# ifndef RECV_BUFFER_SIZE
#  define RECV_BUFFER_SIZE 1024
# endif
#include <string>

extern bool SERVER_RUNNING;

class Server;
class Client;

#include "IRCUtils.hpp"
#include "cmd/CmdInterface.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "ResponseMsg.hpp"

typedef std::map<const std::string, CmdInterface*> ServerCommandsMap;
typedef std::map<std::string, Channel*> ChannelsMap;

class Server {
// Assignable from outside
private:
	unsigned short port;
	std::string password;
	std::string host;
	unsigned int maxConnections;

// Server-only members
private:
	std::string retrievedHostname;
	/// Map of implemented commands (key: command name, value: command object)
	ServerCommandsMap commands;

	/// Map of currently connected clients (key: client fd, value: client object)
	ClientsMap clients;

	/// Socket file descriptor
	SocketFd socketFd;
	/// Socket address
	SocketAddrIn socketAddr;
	/// polling fds
	AllPollFdsVector allPollFds;

	// CHANNELS
	ChannelsMap channels;

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

	/// Send a message to the client
	bool sendMessageToClient(Client* client, const std::string& message) const;

	//bool sendMessageToChannel(Channel *channel, const std::string& message) const;

public:
	Server(const std::string& host, const std::string& port, const std::string& password);
	~Server();
	void run();

	bool isPasswordValid(const std::string& passwordToCheck) const;

	Client* findClientByNickname(const std::string& nickname, bool checkOnlyFullyRegistered) const;

	void notifyClientNicknameChangeToOthers(Client& client, const std::string& newNickname);

	Channel* addChannel(const std::string& name);

	bool sendMessageToChannel(Channel* channel, const std::vector<SocketFd>& excludeClients, const std::string& message) const;

// GETTERS/SETTERS
public:
	const std::string& getRetrievedHostname() const;
	Channel* getChannelByName(const std::string& channelName);
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
