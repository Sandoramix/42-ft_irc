#include "Server.hpp"
#include "cmd/InviteCmd.hpp"
#include "cmd/JoinCmd.hpp"
#include "cmd/KickCmd.hpp"
#include "cmd/ModeCmd.hpp"
#include "cmd/TopicCmd.hpp"

#include <csignal>
#include <cstdlib>
#include <string>


// PRIVATE METHODS ------------------------------------------------------------

void Server::initCommands()
{
	allCommands["INVITE"] = new InviteCmd(*this);
	allCommands["JOIN"] = new JoinCmd(*this);
	allCommands["KICK"] = new KickCmd(*this);
	allCommands["MODE"] = new ModeCmd(*this);
	allCommands["TOPIC"] = new TopicCmd(*this);
}

void Server::initSocket()
{

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
}

// DESTRUCTOR
Server::~Server()
{
	this->allCommands.clear();
}

// START SERVER
void Server::run()
{
	// Server socket creation > bind > listen

	// MAIN LOOP
	SERVER_RUNNING = true;
	while (SERVER_RUNNING) {
		// - accept
		// - parse
		// - run
		// - send
		// ?
	}
}
// EXCEPTIONS -----------------------------------------------------------------
Server::BadConfigException::BadConfigException(const std::string& msg)
		:msg(msg) { }
Server::BadConfigException::~BadConfigException() throw() { }
const char* Server::BadConfigException::what() const throw() { return this->msg.c_str(); }
Server::BadConfigException::BadConfigException(const Server::BadConfigException& b)
		:msg(b.msg) { }
