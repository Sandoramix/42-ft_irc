#include "cmd/CmdInterface.hpp"

CmdInterface::CmdInterface(const std::string& commandName, const Server& server, bool authenticationRequired)
		:authenticationRequired(authenticationRequired), commandName(commandName), server(server)
{
}

CmdInterface::~CmdInterface() { }

void CmdInterface::run(const Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)requestedFrom;
	(void)params;
	throw std::runtime_error("Not implemented.");
}

std::vector<std::string> CmdInterface::parseArgs(const std::string &argsWithoutCommand)
{
	std::vector<std::string> args;

	size_t prev = 0, pos = 0;
	pos = argsWithoutCommand.find(' ', prev);
	while (pos != std::string::npos) {
		args.push_back(argsWithoutCommand.substr(prev, pos-prev));
		prev = pos+1;
		pos = argsWithoutCommand.find(' ', prev);
	}
	return args;
}
bool CmdInterface::canUserRun(const Client& requestedFrom) const
{
	if (this->authenticationRequired) {
		return (requestedFrom.isAuthenticated());
	}
	return (true);
}
