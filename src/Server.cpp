#include "Server.hpp"

#include "cmd/InviteCmd.hpp"
#include "cmd/JoinCmd.hpp"
#include "cmd/KickCmd.hpp"
#include "cmd/ModeCmd.hpp"
#include "cmd/TopicCmd.hpp"


// PRIVATE METHODS ------------------------------------------------------------

void Server::initCommands()
{
	commands["INVITE"] = new InviteCmd(*this);
	commands["JOIN"] = new JoinCmd(*this);
	commands["KICK"] = new KickCmd(*this);
	commands["MODE"] = new ModeCmd(*this);
	commands["TOPIC"] = new TopicCmd(*this);
}

void Server::startListening()
{
	this->socketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (this->socketFd<=0) {
		throw ServerException("Socket creation failed");
	}

	int optval = 1;
	if (setsockopt(this->socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0) {
		throw ServerException("Failed to set socket option");
	}

	if (fcntl(this->socketFd, F_SETFL, O_NONBLOCK) == -1) {
		throw ServerException("Failed to set socket as non-blocking");
	}

	this->socketAddr.sin_family = AF_INET;
	this->socketAddr.sin_addr.s_addr = inet_addr(this->host.c_str());
	this->socketAddr.sin_port = htons(this->port);


	if (bind(this->socketFd, (struct sockaddr*)&this->socketAddr, sizeof(this->socketAddr)) < 0) {
		throw ServerException("Failed to bind socket");
	}
	if (listen(this->socketFd, (int)this->maxConnections) < 0){
		throw ServerException("Failed to listen on socket");
	}

	this->serverPollFd = pollfd();
	this->serverPollFd.fd = this->socketFd;
	this->serverPollFd.events = POLLIN;
}

void Server::acceptConnection()
{
	// Check if server is full: if the users don't exceed the maxConnections.
	if (this->clients.size() >= this->maxConnections) {
		std::cerr << "Server is full: cannot accept new connection." << std::endl;
		return;
	}

	SocketAddrIn addr;
	socklen_t addr_len = sizeof(addr);
	int clientFd = accept(this->socketFd, (SocketAddr *)&addr, &addr_len);
	if (clientFd == -1) {
		std::cerr << "Someone tried to connect but the `accept` failed..." << std::endl;
		return;
	}
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Could not accept a new user because an error occurred when trying to set the socket as non-blocking" << std::endl;
		return;
	}
	// save the user somewhere
	// this->usersMap[clientFd] = new User(...)
	if (this->clients.find(clientFd) != this->clients.end()) {
		delete this->clients[clientFd];
	}
	this->clients[clientFd] = new Client(clientFd);

	PollFd poll = pollfd();
	poll.fd = clientFd;
	poll.events = POLLIN;
	this->clientPollFds.push_back(poll);

	// std::cout << "New connection accepted" << std::endl;
}

void Server::receiveClientMessage(Client *client)
{
	char buffer[RECV_BUFFER_SIZE + 1];
	int bytesRead = 0;
	bool disconnected = false;

	do {
		bytesRead = recv(client->getSocketFd(), buffer, RECV_BUFFER_SIZE, 0);
		if (bytesRead == -1) {
			client->setLocalBuffer("");
			std::cerr << "Error while receiving data from client." << std::endl;
			return;
		}
		buffer[bytesRead] = 0;
		if (bytesRead == 0 && client->getLocalBuffer().empty()) {
			// TODO: find a way to disconnect client
			//std::cerr << "Client disconnected." << std::endl;
			//disconnected = true;
			break;
		}
		client->setLocalBuffer(client->getLocalBuffer() + buffer);
	} while (bytesRead > 0);
	if (disconnected) {
		client->setState(CS_DISCONNECTED);
		return;
	}
}

// PUBLIC METHODS -------------------------------------------------------------

bool Server::tryToRunClientCommand(Client *client)
{
	bool atleastOneCommand = false;
	std::string endDelimiter = "\r\n";
	size_t pos;


	pos = client->getLocalBuffer().find(endDelimiter);
	while (pos != std::string::npos) {
		std::string command = client->getLocalBuffer().substr(0, pos);
		client->setLocalBuffer(client->getLocalBuffer().substr(pos+endDelimiter.size()));
		atleastOneCommand = true;

		// Get command name
		size_t firstSpace = command.find(' ');
		if (firstSpace == std::string::npos) {
			std::cerr << "Invalid command received from client." << std::endl;
			// TODO: send error message
			return false;
		}
		std::string commandName = command.substr(0, firstSpace);
		command = command.substr(firstSpace+1);
		if (this->commands.find(commandName) == this->commands.end()) {
			std::cerr << "Unknown command received from client." << std::endl;
			// TODO: send error message
			return false;
		}
		CmdInterface *cmd = this->commands[commandName];

		// Get command parameters (may be different for some commands (e.g. PASS))
		std::vector<std::string> params = cmd->parseArgs(command);

		cmd->run(*client, params);
	};
	return atleastOneCommand;
}

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

		if (this->serverPollFd.revents == POLLIN) {
			this->acceptConnection();
		} else {
			for (ClientPollVector::iterator clientPoll = this->clientPollFds.begin(); clientPoll != this->clientPollFds.end(); ++clientPoll) {
				if (clientPoll->events == POLLIN) {
					// receive && parse message
					this->receiveClientMessage(this->clients[clientPoll->fd]);
					this->tryToRunClientCommand(this->clients[clientPoll->fd]);
				}
			}
		}
		//Remove disconnected clients
		for (ClientsMap::iterator it = this->clients.begin(); it!=this->clients.end(); ++it) {
			if (it->second->getState() == CS_DISCONNECTED) {
				delete it->second;
				this->clients.erase(it);
			}
		}
	}
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
