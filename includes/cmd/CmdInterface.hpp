#ifndef IRCCMD_HPP
#define IRCCMD_HPP

#include <vector>
#include <stdexcept>

class CmdInterface;

#include "Client.hpp"
#include "Server.hpp"

/**
 * Parent class for all commands.
 * Provides common functionality for all commands.
 * Every command will be instantiated by the server at startup.
*/
class CmdInterface {
protected:
	bool authenticationRequired;
	const std::string commandName;
	const Server& server;

public:
	CmdInterface(const std::string& commandName, const Server& server, bool authenticationRequired = true);

	virtual ~CmdInterface();

	virtual void run(const Client& requestedFrom, const std::vector<std::string>& params);

	/// Try to parse the command arguments from the client buffer. It is virtual so that some commands can override it.
	virtual std::vector<std::string> parseArgs(const std::string &argsWithoutCommand);

protected:
	virtual bool canUserRun(const Client& requestedFrom) const;
};

#endif //IRCCMD_HPP
