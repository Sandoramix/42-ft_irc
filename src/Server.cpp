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
#include "cmd/WhoCmd.hpp"

void Server::initServerIp()
{
	SocketFd tempSocketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (tempSocketFd<=0) {
		throw ServerException("Server IP retrieval: failed to create socket");
	}

	// Fake connection to 8.8.8.8 (Google's DNS)
	SocketAddrIn tempSocketAddr;
	tempSocketAddr.sin_family = AF_INET;
	tempSocketAddr.sin_addr.s_addr = inet_addr("8.8.8.8");
	tempSocketAddr.sin_port = htons(53);

	if (connect(tempSocketFd, (struct sockaddr*)&tempSocketAddr, sizeof(tempSocketAddr))<0) {
		close(tempSocketFd);
		throw ServerException("Server IP retrieval: failed to connect to 8.8.8.8");
	}

	SocketAddrIn localAddr;
	socklen_t localAddrLen = sizeof(localAddr);
	if (getsockname(tempSocketFd, (struct sockaddr*)&localAddr, &localAddrLen)<0) {
		close(tempSocketFd);
		throw ServerException("Server IP retrieval: failed to get socket name");
	}
	char* ipStr = inet_ntoa(localAddr.sin_addr);
	if (!ipStr) {
		close(tempSocketFd);
		throw ServerException("Server IP retrieval: failed to convert IP to string");
	}
	std::string result(ipStr);

	close(tempSocketFd);

	this->host = result;
	ResponseMsg::setHostname(result);

}

// CONSTRUCTOR
Server::Server(const std::string& port, const std::string& password)
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


	this->maxConnections = IRC_MAX_CONNECTIONS;

	this->initServerIp();

	debug("Server configured with port=" << this->port << ", password=" << this->password << ", maxConnections=" << this->maxConnections);
}

// DESTRUCTOR
Server::~Server()
{
	for (ServerCommandsMap::iterator it = this->commandsMap.begin(); it!=this->commandsMap.end(); ++it) {
		delete it->second;
	}
	this->commandsMap.clear();

	for (ChannelsMap::iterator it = this->channelsMap.begin(); it!=this->channelsMap.end(); ++it) {
		delete it->second;
	}
	for (ClientsMap::iterator it = this->clientsMap.begin(); it!=this->clientsMap.end(); ++it) {
		Client* client = it->second;
		if (client) {
			close(client->getSocketFd());
			delete client;
		}
	}
	this->clientsMap.clear();
}

// PRIVATE METHODS ------------------------------------------------------------

void Server::initCommands()
{
	commandsMap["PASS"] = new PassCmd(*this);
	commandsMap["NICK"] = new NickCmd(*this);
	commandsMap["USER"] = new UserCmd(*this);

	commandsMap["INVITE"] = new InviteCmd(*this);
	commandsMap["JOIN"] = new JoinCmd(*this);
	commandsMap["KICK"] = new KickCmd(*this);
	commandsMap["MODE"] = new ModeCmd(*this);
	commandsMap["TOPIC"] = new TopicCmd(*this);

	commandsMap["PRIVMSG"] = new PrivMsgCmd(*this);

	// EXTRA COMMANDS
	commandsMap["WHO"] = new WhoCmd(*this);
	commandsMap["PING"] = new PingCmd(*this);

	debug("Initialized " << commandsMap.size() << " commandsMap.");
}

void Server::startListening()
{
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
	debugInfo("Server: socket created && bound && listening. socketFd: " << this->socketFd << ", host: " << this->host);
}

void Server::acceptConnection()
{
	SocketAddrIn addr;
	socklen_t addr_len = sizeof(addr);
	SocketFd clientFd = accept(this->socketFd, (SocketAddr*)&addr, &addr_len);
	if (clientFd==-1) {
		std::cerr << RED << "Someone tried to connect but the `accept` failed..." << RESET << std::endl;
		return;
	}

	// Check if server is full: if the users don't exceed the maxConnections.
	if (this->clientsMap.size()>=this->maxConnections) {
		std::cerr << RED << "Server is full: cannot accept new connection." << RESET << std::endl;
		send(clientFd, ":Server is full", 21, 0);
		close(clientFd);
		return;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK)==-1) {
		std::cerr << RED << "Could not accept a new user because an error occurred when trying to set the socket as non-blocking" << RESET << std::endl;
		send(clientFd, ":A server side error occurred", 25, 0);
		close(clientFd);
		return;
	}
	if (this->clientsMap.find(clientFd)!=this->clientsMap.end()) {
		delete this->clientsMap[clientFd];
	}
	Client* client = new Client(clientFd);
	this->clientsMap[clientFd] = client;

	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	const char* clientIp = inet_ntoa(addr.sin_addr);

	if (!clientIp) {
		std::cerr << "inet_ntoa failed" << std::endl;
		return;
	}

	std::string clientHostname(clientIp);

	if (clientHostname == "127.0.0.1") {
		clientHostname = this->host;
		addr.sin_addr.s_addr = inet_addr(this->host.c_str());
	}

	std::cout << "Hostname: [" << clientHostname << "]" << std::endl;



	client->setHostname(clientHostname);

	this->allPollFds.push_back(pollfd());
	this->allPollFds.back().fd = clientFd;
	this->allPollFds.back().events = POLLIN | POLLHUP;

	debugSuccess("Client[" << clientFd << "]: new user connected from " << this->clientsMap[clientFd]->getHostname());
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

bool Server::runClientCommands(Client* client)
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
		if (this->commandsMap.find(commandName)==this->commandsMap.end()) {
			client->sendMessage(ResponseMsg::errorResponse(ERR_UNKNOWNCOMMAND, client->getNickname(), commandName, "command not found"));

			debugError("Client[" << client->getSocketFd() << "] tried to run unknown command [" << commandName << "] with arguments \"" << commandArgs << "\"");

			findNextDelimiter(client->getLocalBuffer(), pos, delimSize);
			continue;
		}
		CmdInterface* cmd = this->commandsMap[commandName];
		commandCount++;

		try {
			std::vector<std::string> params = cmd->parseArgs(commandArgs);
			debug("Client[" << client->getSocketFd() << "]: command [" << commandName << "] parsed successfully. Args=" << params << " (size=" << params.size() << ")");

			cmd->checkForAuthOrSendErrorAndThrow(*client);

			cmd->run(*client, params);
		}
		catch (CmdInterface::CmdSyntaxErrorException& e) {
			debugError("Client[" << client->getSocketFd() << "] tried to run command [" << commandName << "] with arguments \"" << commandArgs << "\" but provided invalid arguments. Error: "
								 << e.what());
			client->sendMessage(ResponseMsg::errorResponse(ERR_NEEDMOREPARAMS, client->getNickname(), "", e.what()));
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
	if (!channel) {
		return false;
	}
	ClientsVector channelClients = channel->getAllClients();
	bool error = false;
	for (ClientsVector::iterator it = channelClients.begin(); it!=channelClients.end(); ++it) {
		if (!*it) { continue; }
		// Skip clients that are in the exclude list
		Client* client = *it;
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
	ClientsMap::iterator clientMapIt = this->clientsMap.begin();
	while (clientMapIt!=this->clientsMap.end()) {
		Client* client = clientMapIt->second;
		if (client && client->getState()==CS_DISCONNECTED) {
			debug("Client[" << clientMapIt->first << "] disconnected. Deleting client...");
			// Close socket && delete client
			close(clientMapIt->first);

			// Delete pollfd
			for (AllPollFdsVector::iterator clientPoll = this->allPollFds.begin(); clientPoll!=this->allPollFds.end(); ++clientPoll) {
				if (clientPoll->fd==clientMapIt->first) {
					this->allPollFds.erase(clientPoll);
					break;
				}
			}
			int removedChannelCount = 0;
			for (ChannelsMap::iterator channelMapIt = this->channelsMap.begin(); channelMapIt!=this->channelsMap.end(); ++channelMapIt) {
				Channel* channel = channelMapIt->second;
				if (!channel) { continue; }
				if (channel->removeClient(client)) {
					removedChannelCount++;
				}
			}
			debug("Client[" << clientMapIt->first << "] disconnected. Removed from " << removedChannelCount << " channelsMap.");
			this->clientsMap.erase(clientMapIt++);

			delete client;
		}
		else {
			clientMapIt++;
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
						if (this->clientsMap.find(clientPoll->fd)!=this->clientsMap.end()) {
							this->clientsMap[clientPoll->fd]->setState(CS_DISCONNECTED);
						}
					}
					else if (clientPoll->revents & POLLIN) {
						// receive && parse message
						debugInfo("Client[" << clientPoll->fd << "] has sent a message. Receiving...");
						this->receiveClientMessage(this->clientsMap[clientPoll->fd]);
						this->runClientCommands(this->clientsMap[clientPoll->fd]);
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

Client* Server::findClientByNickname(const std::string& nickname, bool checkOnlyFullyRegistered) const
{
	for (ClientsMap::const_iterator it = this->clientsMap.begin(); it!=this->clientsMap.end(); ++it) {
		if (IRCUtils::caseInsensitiveStringCompare(it->second->getNickname(), nickname) && (!checkOnlyFullyRegistered || it->second->isFullyRegistered())) {
			return it->second;
		}
	}
	return NULL;
}

void Server::notifyClientNicknameChangeToOthers(Client& client, const std::string& newNickname)
{
	ClientsMap receivers;

	receivers[client.getSocketFd()] = &client;
	for (ChannelsMap::iterator it = this->channelsMap.begin(); it!=this->channelsMap.end(); ++it) {
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

	for (ClientsMap::iterator it = receivers.begin(); it!=receivers.end(); ++it) {
		this->sendMessageToClient(it->second, ResponseMsg::nicknameChangeResponse(client, newNickname));
	}
	debugResponse(ResponseMsg::nicknameChangeResponse(client, newNickname));
}

Channel* Server::addChannel(const std::string& name)
{
	if (getChannelByName(name)==NULL)
		this->channelsMap[name] = new Channel(*this, name, "");
	return this->channelsMap[name];
}


// GETTERS/SETTERS ------------------------------------------------------------

const std::string& Server::getRetrievedHostname() const { return this->host; }

Channel* Server::getChannelByName(const std::string& channelName)
{
	if (this->channelsMap.find(channelName)==this->channelsMap.end())
		return NULL;
	return this->channelsMap[channelName];
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
