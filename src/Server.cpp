#include <unistd.h>
#include <cerrno>
#include <climits>
#include "Server.hpp"

#include "cmd/InviteCmd.hpp"
#include "cmd/JoinCmd.hpp"
#include "cmd/KickCmd.hpp"
#include "cmd/ModeCmd.hpp"
#include "cmd/TopicCmd.hpp"
#include "cmd/PassCmd.hpp"
#include "cmd/NickCmd.hpp"
#include "cmd/UserCmd.hpp"

// CONSTRUCTOR
Server::Server(const std::string& host, const std::string& port, const std::string& password)
{
	// PORT VALIDATION
	if (port.empty()) { throw BadConfigException("Invalid port: empty string."); }
	char* end;
	long port_l = strtol(port.c_str(), &end, 10);
	if (end==port.c_str() || *end!='\0') { throw BadConfigException("Invalid port: not a number."); }
	if (port_l<1 || port_l>65535) { throw BadConfigException("Invalid port: out of range."); }

	this->port = static_cast<unsigned short>(port_l);

	// PASSWORD VALIDATION
	if (password.empty()) { throw BadConfigException("Invalid password: empty string."); }
	this->password = password;

	// HOST VALIDATION
	if (host.empty()) { throw BadConfigException("Invalid host: empty string."); }
	this->host = host;

	this->maxConnections = IRC_MAX_CONNECTIONS;
	debug("Server configured with host=" << this->host << ", port=" << this->port << ", password=" << this->password << ", maxConnections=" << this->maxConnections);
}

// DESTRUCTOR
Server::~Server()
{
	for (ServerCommandsMap::iterator it = this->commands.begin(); it!=this->commands.end(); ++it) {
		delete it->second;
	}
	this->commands.clear();

	for (ClientsMap::iterator it = this->clients.begin(); it!=this->clients.end(); ++it) {
		delete it->second;
	}
	this->clients.clear();
}

// PRIVATE METHODS ------------------------------------------------------------

void Server::initCommands()
{
	commands["PASS"] = new PassCmd(*this);
	commands["NICK"] = new NickCmd(*this);
	commands["USER"] = new UserCmd(*this);

	commands["INVITE"] = new InviteCmd(*this);
	commands["JOIN"] = new JoinCmd(*this);
	commands["KICK"] = new KickCmd(*this);
	commands["MODE"] = new ModeCmd(*this);
	commands["TOPIC"] = new TopicCmd(*this);

	debug("Initialized " << commands.size() << " commands.");
}

void Server::startListening()
{
	char hostname[HOST_NAME_MAX];
	if (gethostname(hostname, HOST_NAME_MAX)<0) {
		throw ServerException("Failed to retrieve hostname");
	}
	this->retrievedHostname = hostname;
	ResponseMsg::setHostname(this->retrievedHostname);

	this->socketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (this->socketFd<=0) {
		throw ServerException("Socket creation failed");
	}

	int optval = 1;
	if (setsockopt(this->socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))!=0) {
		throw ServerException("Failed to set socket option");
	}

	if (fcntl(this->socketFd, F_SETFL, O_NONBLOCK)==-1) {
		throw ServerException("Failed to set socket as non-blocking");
	}

	this->socketAddr.sin_family = AF_INET;
	this->socketAddr.sin_addr.s_addr = inet_addr(this->host.c_str());
	this->socketAddr.sin_port = htons(this->port);

	if (bind(this->socketFd, (struct sockaddr*)&this->socketAddr, sizeof(this->socketAddr))<0) {
		throw ServerException("Failed to bind socket");
	}
	if (listen(this->socketFd, (int)this->maxConnections)<0) {
		throw ServerException("Failed to listen on socket");
	}

	this->allPollFds.push_back(pollfd());
	this->allPollFds.back().fd = this->socketFd;
	this->allPollFds.back().events = POLLIN;
	debug("Socket created && bound && listening. Socket fd: " << this->socketFd);
}

void Server::acceptConnection()
{
	// Check if server is full: if the users don't exceed the maxConnections.
	if (this->clients.size()>=this->maxConnections) {
		std::cerr << "Server is full: cannot accept new connection." << std::endl;
		return;
	}

	SocketAddrIn addr;
	socklen_t addr_len = sizeof(addr);

	SocketFd clientFd = accept(this->socketFd, (SocketAddr*)&addr, &addr_len);
	if (clientFd==-1) {
		std::cerr << "Someone tried to connect but the `accept` failed..." << std::endl;
		return;
	}
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK)==-1) {
		std::cerr << "Could not accept a new user because an error occurred when trying to set the socket as non-blocking" << std::endl;
		return;
	}
	if (this->clients.find(clientFd)!=this->clients.end()) {
		delete this->clients[clientFd];
	}
	this->clients[clientFd] = new Client(clientFd);

	this->allPollFds.push_back(pollfd());
	this->allPollFds.back().fd = clientFd;
	this->allPollFds.back().events = POLLIN | POLLHUP;

	debug("New user connected. Socket fd: " << clientFd);
}

void Server::receiveClientMessage(Client* client)
{
	char buffer[RECV_BUFFER_SIZE+1];
	ssize_t bytesRead = 0;
	bool disconnected = false;

	bytesRead = recv(client->getSocketFd(), buffer, RECV_BUFFER_SIZE, 0);
	if (bytesRead>0) {
		buffer[bytesRead] = 0;
		client->setLocalBuffer(client->getLocalBuffer()+buffer);
	}
	else if (bytesRead==0) {
		// Client closed connection
		disconnected = true;
	}
	else if (bytesRead==-1) {
		if (errno!=EAGAIN && errno!=EWOULDBLOCK) {
			debug("Error in recv() for client[" << client->getSocketFd() << "]");
			disconnected = true;
		}
	}

	if (disconnected) {
		client->setState(CS_DISCONNECTED);
		return;
	}
}

static void findNextDelimiter(const std::string& str, size_t& pos, size_t& delimSize)
{
	std::string unixEndDelimiter = "\r\n";
	std::string windowsEndDelimiter = "\n";
	delimSize = unixEndDelimiter.size();

	pos = str.find(unixEndDelimiter);
	delimSize = unixEndDelimiter.size();
	if (pos==std::string::npos) {
		pos = str.find(windowsEndDelimiter);
		delimSize = windowsEndDelimiter.size();
	}
}

bool Server::tryToRunClientCommand(Client* client)
{
	int commandCount = 0;

	size_t pos;
	size_t delimSize;

	if (client->getState()==CS_DISCONNECTED) {
		return false;
	}

	findNextDelimiter(client->getLocalBuffer(), pos, delimSize);

	while (pos!=std::string::npos) {
		std::string commandName;
		std::string commandArgs = client->getLocalBuffer().substr(0, pos);
		client->setLocalBuffer(client->getLocalBuffer().substr(pos+delimSize));

		// Get command name
		size_t firstSpace = commandArgs.find(' ');
		if (firstSpace==std::string::npos) {
			commandName = commandArgs;
			commandArgs = "";
		}
		else {
			commandName = commandArgs.substr(0, firstSpace);
			commandArgs = commandArgs.substr(firstSpace+1);
		}
		if (this->commands.find(commandName)==this->commands.end()) {
			debug("Unknown command received from client[" << client->getSocketFd() << "]. commandName=" << commandName << ", command=" << commandArgs);
			// TODO: send error message to client
			findNextDelimiter(client->getLocalBuffer(), pos, delimSize);
			continue;
		}
		commandCount++;
		CmdInterface* cmd = this->commands[commandName];

		// Get command parameters (may be different for some commands (e.g. PASS))
		std::vector<std::string> params = cmd->parseArgs(commandArgs);
		debug("Command received from client[" << client->getSocketFd() << "]. commandName=" << commandName << ", args=" << params);

		cmd->run(*client, params);
		findNextDelimiter(client->getLocalBuffer(), pos, delimSize);
	};
	debug("Command parsing finished. Commands found=" << commandCount);
	return commandCount>0;
}

bool Server::sendMessageToClient(Client* client, const std::string& message) const
{
	if (!client) { return false; }
	return client->sendMessage(message);
}

bool Server::sendMessageToChannel(Channel* channel, const std::string& message) const
{
	// TODO: add checks for channel permissions and so on.
	if (!channel) {
		return false;
	}
	ClientsVector channelClients = channel->getAllClients();
	bool error = false;
	for (ClientsVector::iterator it = channelClients.begin(); it!=channelClients.end(); ++it) {
		bool sent = this->sendMessageToClient(*it, message);
		if (!sent) {
			debug("Error while sending message to channel[" << channel->getName() << "]. Client[" << (*it)->getSocketFd() << "]");
			error = true;
		}
	}
	return !error;
}

void Server::deleteDisconnectedClients()
{
	ClientsMap::iterator client = this->clients.begin();
	while (client!=this->clients.end()) {
		if (client->second && client->second->getState()==CS_DISCONNECTED) {
			debug("FD[" << client->first << "] Client disconnected. Deleting client...");
			// Close socket && delete client
			close(client->first);
			delete client->second;
			client->second = NULL;

			// Delete pollfd
			for (AllPollFdsVector::iterator clientPoll = this->allPollFds.begin(); clientPoll!=this->allPollFds.end(); ++clientPoll) {
				if (clientPoll->fd==client->first) {
					this->allPollFds.erase(clientPoll);
					break;
				}
			}
			this->clients.erase(client++);
			// TODO: remove client from it's channels
		}
		else {
			client++;
		}
	}
}

// PUBLIC METHODS -------------------------------------------------------------

// START SERVER
void Server::run()
{
	this->initCommands();
	this->startListening();

	// MAIN LOOP
	SERVER_RUNNING = true;
	std::cout << "Server running on port " << this->port << "... Press Ctrl+C to stop." << std::endl;
	while (SERVER_RUNNING) {

		// - accept
		// - parse
		// - run
		// - send
		// ?

		// Wait for events (timeout of 1000ms)
		if (poll(this->allPollFds.data(), this->allPollFds.size(), 1000)<0) {
			if (!SERVER_RUNNING) break;
			throw ServerException("Poll failed");
		}

		if (this->allPollFds[0].revents & POLLIN) {
			this->acceptConnection();
		}
		else {
			for (AllPollFdsVector::iterator clientPoll = this->allPollFds.begin(); clientPoll!=this->allPollFds.end(); ++clientPoll) {
				if (clientPoll->revents & (POLLIN | POLLHUP | POLLERR)) {
					if (clientPoll->revents & (POLLHUP | POLLERR)) {
						// Client disconnected forcefully (CTRL+C, etc.)
						if (this->clients.find(clientPoll->fd)!=this->clients.end()) {
							this->clients[clientPoll->fd]->setState(CS_DISCONNECTED);
						}
					}
					else if (clientPoll->revents & POLLIN) {
						// receive && parse message
						debug("Receiving message from fd[" << clientPoll->fd << "]...");
						this->receiveClientMessage(this->clients[clientPoll->fd]);
						this->tryToRunClientCommand(this->clients[clientPoll->fd]);
					}
				}
			}
		}
		//Remove disconnected clients
		this->deleteDisconnectedClients();
	}
}

// OTHER PUBLIC METHODS USED BY THE COMMANDS -----------------------------------

bool Server::isPasswordValid(const std::string& password) const { return this->password==password; }

// GETTERS/SETTERS ------------------------------------------------------------

const std::string& Server::getRetrievedHostname() const { return this->retrievedHostname; }


// EXCEPTIONS -----------------------------------------------------------------

// BAD CONFIG EXCEPTION
Server::BadConfigException::BadConfigException(const std::string& msg)
		:msg(msg) { }
Server::BadConfigException::~BadConfigException() throw() { }
const char* Server::BadConfigException::what() const throw() { return this->msg.c_str(); }

// SERVER EXCEPTION
Server::ServerException::ServerException(const std::string& msg)
		:msg(msg) { }
Server::ServerException::~ServerException() throw() { }
const char* Server::ServerException::what() const throw() { return this->msg.c_str(); }
