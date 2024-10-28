#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <exception>
#include <map>
#include <string>

class IRCServer;

#include "cmd/CmdInterface.hpp"


class IRCServer
{
private:
	unsigned short port;
	std::string    password;

	/// Map of implemented commands (key: command name, value: command object)
	std::map<std::string, CmdInterface &> implementedCommands;

public:
	IRCServer(const std::string &port, const std::string &password);
	void run();

	class BadConfigException : public std::exception
	{
		private:
			std::string msg;
		public:
			BadConfigException(const std::string &msg);
			virtual ~BadConfigException() throw();
			BadConfigException(const BadConfigException &);
			virtual const char *what() const throw();
	};
};

#endif //IRCSERVER_HPP
