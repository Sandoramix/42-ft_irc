#ifndef IRCCMD_HPP
#define IRCCMD_HPP

#include <vector>
#include <stdexcept>

class CmdInterface;

#include "IRCUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "ResponseMsg.hpp"

/**
 * Parent class for all commands.
 * Provides common functionality for all commands.
 * Every command will be instantiated by the server at startup.
*/
class CmdInterface {
protected:
	bool authenticationRequired;
	const std::string commandName;
	Server& server;

public:
	CmdInterface(const std::string& commandName, Server& server, bool authenticationRequired = true);

	virtual ~CmdInterface();

	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);

	/// Try to parse the command arguments from the client buffer. It is virtual so that some commands can override it.
	virtual std::vector<std::string> parseArgs(const std::string& argsWithoutCommand);

	/**
	 * If the command requires authentication, check if the client is authenticated and throw an exception if not.
	 * @param requestedFrom client that is trying to run the command
	 * @return true if the client is authenticated, false otherwise (exception is thrown)
	 */
	virtual bool checkForAuthOrSendErrorAndThrow(Client& requestedFrom) const;

public:
	class CmdSyntaxErrorException : public std::exception {
	private:
		std::string msg;
	public:
		explicit CmdSyntaxErrorException(const std::string& specificReason);
		virtual ~CmdSyntaxErrorException() throw();
		virtual const char* what() const throw();
	};

	class CmdAuthErrorException : public std::exception {
	private:
		std::string msg;
	public:
		explicit CmdAuthErrorException(const std::string& specificReason);
		virtual ~CmdAuthErrorException() throw();
		virtual const char* what() const throw();
	};
};

#endif //IRCCMD_HPP
