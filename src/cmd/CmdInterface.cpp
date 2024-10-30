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

bool CmdInterface::canUserRun(const Client& requestedFrom) const
{
	if (this->authenticationRequired) {
		return (requestedFrom.isAuthenticated());
	}
	return (true);
}
