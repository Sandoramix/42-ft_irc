#include "cmd/CmdInterface.hpp"

CmdInterface::CmdInterface(const std::string& commandName, Server& server, bool authenticationRequired)
		:authenticationRequired(authenticationRequired), commandName(commandName), server(server)
{
}

CmdInterface::~CmdInterface() { }

void CmdInterface::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)requestedFrom;
	(void)params;
	throw std::runtime_error("CmdInterface::run() not implemented. This is a bug because it doesn't make sense to call this method.");
}

std::vector<std::string> CmdInterface::parseArgs(const std::string& argsWithoutCommand)
{
	std::string copy(argsWithoutCommand);
	std::vector<std::string> args;

	size_t colonPos = copy.find(':');

	size_t pos = 0;
	pos = copy.find(' ', 0);
	while (pos!=std::string::npos && (colonPos==std::string::npos || pos<colonPos)) {
		args.push_back(copy.substr(0, pos));
		copy = copy.substr(pos+1);
		pos = copy.find(' ', 0);
		colonPos = copy.find(':');
	}
	if (colonPos!=std::string::npos) {
		std::string preColonArg = copy.substr(0, colonPos);
		if (!preColonArg.empty()) {
			args.push_back(preColonArg);
		}
		copy = copy.substr(colonPos+1);
		args.push_back(copy);
		copy = "";
	}
	if (!copy.empty()) {
		args.push_back(copy);
	}
	return args;
}

bool CmdInterface::checkForAuthOrSendErrorAndThrow(Client& requestedFrom) const
{
	if (this->authenticationRequired) {
		if (!requestedFrom.isFullyRegistered()) {
			throw CmdInterface::CmdSyntaxErrorException("Not authenticated");
		}
		return (true);
	}
	return (true);
}


// EXCEPTIONS -----------------------------------------------------------------

CmdInterface::CmdSyntaxErrorException::CmdSyntaxErrorException(const std::string& specificReason) { this->msg = specificReason; }
CmdInterface::CmdSyntaxErrorException::~CmdSyntaxErrorException() throw() { }
const char* CmdInterface::CmdSyntaxErrorException::what() const throw() { return this->msg.c_str(); }

CmdInterface::CmdAuthErrorException::CmdAuthErrorException(const std::string& specificReason) { this->msg = specificReason; }
CmdInterface::CmdAuthErrorException::~CmdAuthErrorException() throw() { }
const char* CmdInterface::CmdAuthErrorException::what() const throw() { return this->msg.c_str(); }
