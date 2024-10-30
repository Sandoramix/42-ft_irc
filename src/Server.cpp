#include "Server.hpp"
#include "cmd/InviteCmd.hpp"
#include "cmd/JoinCmd.hpp"
#include "cmd/KickCmd.hpp"
#include "cmd/ModeCmd.hpp"
#include "cmd/TopicCmd.hpp"

#include <csignal>
#include <cstdlib>
#include <string>
#include <arpa/inet.h>
#include <iostream>


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

	this->socketAddr.sin_family = AF_INET;
	this->socketAddr.sin_addr.s_addr = inet_addr(this->host.c_str());
	this->socketAddr.sin_port = htons(this->port);

	int optval = 1;
	if (setsockopt(this->socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		throw ServerException("Failed to set socket option");
	}

	if (bind(this->socketFd, (struct sockaddr*)&this->socketAddr, sizeof(this->socketAddr))<0) {
		throw ServerException("Failed to bind socket");
	}
	if (listen(this->socketFd, (int)this->maxConnections) < 0){
		throw ServerException("Failed to listen on socket");
	}
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
