#include "IRCServer.hpp"

#include <cstdlib>
#include <string>

IRCServer::IRCServer(const std::string &port, const std::string &password)
{
	// PORT VALIDATION
	if (port.empty()) { throw BadConfigException("Invalid port: empty string."); }
	char *end;
	long  port_l = strtol(port.c_str(), & end, 10);
	if (end == port.c_str() || * end != '\0') { throw BadConfigException("Invalid port: not a number."); }
	if (port_l < 1 || port_l > 65535) { throw BadConfigException("Invalid port: out of range."); }

	this->port = static_cast<unsigned short>(port_l);

	// PASSWORD VALIDATION
	if (password.empty()) { throw BadConfigException("Invalid password: empty string."); }
	this->password = password;
}

void IRCServer::run()
{
	// Server socket creation > bind > listen
	// MAIN LOOP
	// - accept
	// - parse
	// - run
	// - send
	// ?
}

// EXCEPTIONS -----------------------------------------------------------------
IRCServer::BadConfigException::BadConfigException(const std::string &msg) : msg(msg)
{
}

IRCServer::BadConfigException::~BadConfigException() throw()
{
}

const char *IRCServer::BadConfigException::what() const throw()
{
	return this->msg.c_str();
}
