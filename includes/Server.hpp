#ifndef SERVER_HPP
#define SERVER_HPP

# ifndef IRC_MAX_CONNECTIONS
#  define IRC_MAX_CONNECTIONS 420
# endif
# ifndef RECV_BUFFER_SIZE
#  define RECV_BUFFER_SIZE 1024
# endif

extern bool SERVER_RUNNING;

#include "IRCUtils.hpp"

#include "cmd/CmdInterface.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "ResponseMsg.hpp"


class Server {
private:
	unsigned short port;
	std::string password;

	unsigned int maxConnections;

// Server-only members
private:
	std::string host;
	/// Map of implemented commands (key: command name, value: command instance)
	ServerCommandsMap commandsMap;

	/// Channels map
	ChannelsMap channelsMap;

	/// Map of currently connected clients (key: client fd, value: client object)
	ClientsMap clientsMap;

	/// Socket file descriptor
	SocketFd socketFd;
	/// Socket address
	SocketAddrIn socketAddr;

	/// polling fds
	AllPollFdsVector allPollFds;

private:
	void initServerIp();

	/// Initialize commands
	void initCommands();

	/// Start listening: bind socket and listen. Create server pollfd.
	void startListening();

	/// Accept connection: Add new user
	void acceptConnection();

	/// Parse data received from client
	void receiveClientMessage(Client* client);

	/// Try to parse the client buffer as a command if the buffer contains a complete command (ends with '\r\n')
	bool runClientCommands(Client* client);

	/// Delete disconnected clients
	void deleteDisconnectedClients();

	/// Send a message to the client
	bool sendMessageToClient(Client* client, const std::string& message) const;

	//bool sendMessageToChannel(Channel *channel, const std::string& message) const;

public:
	Server(const std::string& port, const std::string& password);
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
