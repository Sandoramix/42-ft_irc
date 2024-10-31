#include "Server.hpp"
#include "cmd/InviteCmd.hpp"
#include "cmd/JoinCmd.hpp"
#include "cmd/KickCmd.hpp"
#include "cmd/ModeCmd.hpp"
#include "cmd/TopicCmd.hpp"



// PRIVATE METHODS ------------------------------------------------------------

void Server::initCommands()
{
	allCommands["INVITE"] = new InviteCmd(*this);
	allCommands["JOIN"] = new JoinCmd(*this);
	allCommands["KICK"] = new KickCmd(*this);
	allCommands["MODE"] = new ModeCmd(*this);
	allCommands["TOPIC"] = new TopicCmd(*this);
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
	(void)this->maxConnections;

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

	PollFd poll = pollfd();
	poll.fd = clientFd;
	poll.events = POLLIN;
	this->clientPollFds.emplace_back(poll);

	// std::cout << "New connection accepted" << std::endl;
}

// PUBLIC METHODS -------------------------------------------------------------

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
	for (ServerCommandsMap::iterator it = this->allCommands.begin(); it!=this->allCommands.end(); ++it) {
		delete it->second;
	}
	this->allCommands.clear();
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
				}
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
