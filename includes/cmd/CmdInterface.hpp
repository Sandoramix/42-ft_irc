#ifndef IRCCMD_HPP
#define IRCCMD_HPP

#include <vector>
#include <stdexcept>


class CmdInterface;

#include "Client.hpp"
#include "IRCServer.hpp"

/**
 * Parent class for all commands.
 * Provides common functionality for all commands.
 * Every command will be instantiated by the server at startup.
*/
class CmdInterface
{
protected:
	bool              authenticationRequired;
	const std::string commandName;
	const IRCServer & server;

public:
	CmdInterface(const std::string &commandName, const IRCServer &server, bool authenticationRequired = true);
	virtual ~CmdInterface();

	virtual void run(const Client &requestedFrom, const std::vector<std::string> &params);

protected:
	bool canUserRun(const Client &requestedFrom) const;
};


#endif //IRCCMD_HPP
