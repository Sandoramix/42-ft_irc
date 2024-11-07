#include "cmd/CmdInterface.hpp"

CmdInterface::CmdInterface(const std::string& commandName, Server& server, bool authenticationRequired, bool checkForColon, bool isColonRequired)
		:authenticationRequired(authenticationRequired), commandName(commandName), server(server), checkForColon(checkForColon), isColonRequired(isColonRequired)
{
}

CmdInterface::~CmdInterface() { }

void CmdInterface::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)requestedFrom;
	(void)params;
	throw std::runtime_error("CmdInterface::run() not implemented. This is a bug because it doesn't make sense to call this method.");
}

// TODO: per la ricerca del colon (:) se trovato, lasciarlo come primo carattere dell'ultimo argomento
std::vector<std::string> CmdInterface::parseArgs(const std::string& argsWithoutCommand)
{
	std::string copy(argsWithoutCommand);
	std::vector<std::string> args;

	size_t colonPos = this->checkForColon ? copy.find(':') : std::string::npos;
	if (this->isColonRequired && colonPos==std::string::npos) {
		throw CmdSyntaxErrorException("missing colon");
	}

	size_t pos = 0;
	pos = copy.find(' ', 0);
	while (pos!=std::string::npos && (!this->checkForColon || pos<colonPos)) {
		args.push_back(copy.substr(0, pos));
		copy = copy.substr(pos+1);
		pos = copy.find(' ', 0);
		colonPos = this->checkForColon ? copy.find(':') : std::string::npos;
	}
	if (this->checkForColon && colonPos!=std::string::npos) {
		if (colonPos!=0 && copy[colonPos-1]!=' ') {
			throw CmdSyntaxErrorException("invalid colon position");
		}
		copy = copy.substr(colonPos+1);
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
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTERED, requestedFrom.getNickname(), ""));
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
