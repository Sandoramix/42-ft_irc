#include "cmd/CmdInterface.hpp"

CmdInterface::CmdInterface(const std::string& commandName, const Server& server, bool authenticationRequired)
		:authenticationRequired(authenticationRequired), commandName(commandName), server(server)
{
}

CmdInterface::~CmdInterface() { }

void CmdInterface::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)requestedFrom;
	(void)params;
	throw std::runtime_error("Not implemented.");
}

std::vector<std::string> CmdInterface::parseArgs(const std::string& argsWithoutCommand)
{
	std::string copy(argsWithoutCommand);
	std::vector<std::string> args;

	size_t pos = 0;
	pos = copy.find(' ', 0);
	while (pos!=std::string::npos) {
		args.push_back(copy.substr(0, pos));
		copy = copy.substr(pos+1);
		pos = copy.find(' ', 0);
	}
	if (!copy.empty()) {
		args.push_back(copy);
	}
	return args;
}
bool CmdInterface::canUserRun(Client& requestedFrom) const
{
	if (this->authenticationRequired) {
		return (requestedFrom.isAuthenticated());
	}
	return (true);
}

// EXCEPTIONS -----------------------------------------------------------------

CmdInterface::CmdSyntaxErrorException::CmdSyntaxErrorException(const std::string& specificReason)
{
	if (specificReason.empty()) {
		msg += "Invalid syntax";
	}
	else if (specificReason.find("Invalid syntax")!=std::string::npos) {
		msg = specificReason;
	}
	else {
		msg += "Invalid syntax: "+specificReason;
	}
}
CmdInterface::CmdSyntaxErrorException::~CmdSyntaxErrorException() throw() { }
const char* CmdInterface::CmdSyntaxErrorException::what() const throw() { return this->msg.c_str(); }
