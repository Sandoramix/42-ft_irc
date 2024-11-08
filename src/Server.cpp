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
#include "cmd/PingCmd.hpp"
#include "cmd/PrivMsgCmd.hpp"

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

	for (ChannelsMap::iterator it = this->channels.begin(); it!=this->channels.end(); ++it) {
		delete it->second;
	}
	for (ClientsMap::iterator it = this->clients.begin(); it!=this->clients.end(); ++it) {
		Client* client = it->second;
		if (client) {
			close(client->getSocketFd());
			delete client;
		}
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

	commands["PRIVMSG"] = new PrivMsgCmd(*this);


	commands["PING"] = new PingCmd(*this);

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
	debugInfo("Server: socket created && bound && listening. socketFd: " << this->socketFd);
}

void Server::acceptConnection()
{
	// Check if server is full: if the users don't exceed the maxConnections.
	if (this->clients.size()>=this->maxConnections) {
		std::cerr << RED << "Server is full: cannot accept new connection." << RESET << std::endl;
		return;
	}

	SocketAddrIn addr;
	socklen_t addr_len = sizeof(addr);

	SocketFd clientFd = accept(this->socketFd, (SocketAddr*)&addr, &addr_len);
	if (clientFd==-1) {
		std::cerr << RED << "Someone tried to connect but the `accept` failed..." << RESET<< std::endl;
		return;
	}
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK)==-1) {
		std::cerr << RED << "Could not accept a new user because an error occurred when trying to set the socket as non-blocking" << RESET << std::endl;
		return;
	}
	if (this->clients.find(clientFd)!=this->clients.end()) {
		delete this->clients[clientFd];
	}
	this->clients[clientFd] = new Client(clientFd);

	this->allPollFds.push_back(pollfd());
	this->allPollFds.back().fd = clientFd;
	this->allPollFds.back().events = POLLIN | POLLHUP;

	debugSuccess("Client[" << clientFd << "]: new user connected.");
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
		disconnected = true;
	}
	else if (bytesRead==-1) {
		if (errno!=EAGAIN && errno!=EWOULDBLOCK) {
			debugError("Error in recv() for client[" << client->getSocketFd() << "]");
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
		debug("Client[" << client->getSocketFd() << "]: command parsing skipped because the client is disconnected.");
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
			client->sendMessage(ResponseMsg::genericResponse(ERR_UNKNOWNCOMMAND, client->getNickname(), ""));

			debugError("Client[" << client->getSocketFd() << "] tried to run unknown command [" << commandName << "] with arguments \"" << commandArgs << "\"");

			findNextDelimiter(client->getLocalBuffer(), pos, delimSize);
			continue;
		}
		CmdInterface* cmd = this->commands[commandName];
		commandCount++;

		try {
			std::vector<std::string> params = cmd->parseArgs(commandArgs);
			debug("Client[" << client->getSocketFd() << "]: command [" << commandName << "] parsed successfully. Args=" << params << " (size=" << params.size() << ")");

			cmd->checkForAuthOrSendErrorAndThrow(*client);

			cmd->run(*client, params);
		}
		catch (CmdInterface::CmdSyntaxErrorException& e) {
			debugError("Client[" << client->getSocketFd() << "] tried to run command [" << commandName << "] with arguments \"" << commandArgs << "\" but provided invalid arguments. Error: " << e.what());
			client->sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, client->getNickname(), "", e.what()));
		}
		catch (CmdInterface::CmdAuthErrorException& e) {
			debugError("Client[" << client->getSocketFd() << "] tried to run command [" << commandName << "] with arguments \"" << commandArgs << "\" but is not authenticated");
			client->sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTERED, client->getNickname(), ""));
		}

		findNextDelimiter(client->getLocalBuffer(), pos, delimSize);
	};
	return commandCount>0;
}

bool Server::sendMessageToClient(Client* client, const std::string& message) const
{
	if (!client) { return false; }
	return client->sendMessage(message);
}

bool Server::sendMessageToChannel(Channel* channel, const std::vector<SocketFd>& excludeClients, const std::string& message) const
{
	// TODO: add checks for channel permissions and so on (?).
	if (!channel) {
		return false;
	}
	ClientsVector channelClients = channel->getAllClients();
	bool error = false;
	for (ClientsVector::iterator it = channelClients.begin(); it!=channelClients.end(); ++it) {
		if (!*it) { continue; }
		// Skip clients that are in the exclude list
		Client *client = *it;
		if (std::find(excludeClients.begin(), excludeClients.end(), client->getSocketFd())!=excludeClients.end()) {
			continue;
		}
		bool sent = this->sendMessageToClient(client, message);
		if (!sent) {
			debugWarning("Client[" << client->getSocketFd() << "] tried to send message to channel[" << channel->getName() << "] but the message was not sent");
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
			debug("Client[" << client->first << "] disconnected. Deleting client...");
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
//			for(ChannelsMap::iterator channelMapIt = this->channels.begin(); channelMapIt!=this->channels.end(); ++channelMapIt) {
//				if (!channelMapIt->second) { continue; }
//				debug("Client[" << client->first << "] is getting removed from channel[" << channelMapIt->second->getName() << "]");
//				channelMapIt->second->removeClient(client->second);
//			}
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
						debugInfo("Client[" << clientPoll->fd << "] has sent a message. Receiving...");
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

bool Server::isPasswordValid(const std::string& passwordToCheck) const { return this->password==passwordToCheck; }

Client* Server::findClientByNickname(const std::string& nickname) const
{
	for (ClientsMap::const_iterator it = this->clients.begin(); it!=this->clients.end(); ++it) {
		if (it->second->getNickname()==nickname) {
			return it->second;
		}
	}
	return NULL;
}

void Server::notifyClientNicknameChangeToOthers(Client& client, const std::string& oldNickname)
{
	ClientsMap receivers;

	for (ChannelsMap::iterator it = this->channels.begin(); it!=this->channels.end(); ++it) {
		if (!it->second) { continue; }
		if (it->second->isClientInChannel(&client)) {
			ClientsVector channelClients = it->second->getAllClients();
			for (ClientsVector::iterator c = channelClients.begin(); c!=channelClients.end(); ++c) {
				if (!*c) { continue; }
				Client* currentClient = *c;
				if (!currentClient) { continue; }
				receivers[currentClient->getSocketFd()] = currentClient;
			}
		}
	}
	ClientsMap::iterator clientWithChangedNickIt = receivers.find(client.getSocketFd());
	if (clientWithChangedNickIt!=receivers.end() && clientWithChangedNickIt->second) {
		receivers.erase(clientWithChangedNickIt);
	}
	for (ClientsMap::iterator it = receivers.begin(); it!=receivers.end(); ++it) {
		this->sendMessageToClient(it->second, ResponseMsg::nicknameChangeResponse(oldNickname, client.getNickname()));
	}
	debugResponse(ResponseMsg::nicknameChangeResponse(oldNickname, client.getNickname()));
}

Channel* Server::addChannel(const std::string& name, bool isPrivate)
{
	if (getChannelByName(name)==NULL)
		this->channels[name] = new Channel(*this, name, "", isPrivate);
	return this->channels[name];
}


// GETTERS/SETTERS ------------------------------------------------------------

const std::string& Server::getRetrievedHostname() const { return this->retrievedHostname; }

Channel* Server::getChannelByName(const std::string& channelName)
{
	if (this->channels.find(channelName)==this->channels.end())
		return NULL;
	return this->channels[channelName];
}


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
