#include "cmd/UserCmd.hpp"

UserCmd::UserCmd(const Server& server)
		:CmdInterface("USER", server, false, false, false)
{
}

UserCmd::~UserCmd()
{
}

/**
 * <h3>Syntax</h3>
 * <pre>
 * USER USERNAME HOSTNAME SERVERNAME :REALNAME
 * </pre>
 */
void UserCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (requestedFrom.getState() < CS_PASS_SENT) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTRED, requestedFrom.getNickname()));
		return;
	}
	if (requestedFrom.getIsUserCmdSent()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_ALREADYREGISTRED, requestedFrom.getNickname()));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but the client is already registered");
		return;
	}
	if (params.size()!=4) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "Invalid number of parameters"));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but invalid number of parameters was provided");
		return;
	}

	requestedFrom.setUsername(params[0]);
	requestedFrom.setHostname(params[1]);
	requestedFrom.setServerName(params[2]);
	requestedFrom.setRealName(params[3]);
	requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_WELCOME, requestedFrom.getNickname()));
}
std::vector<std::string> UserCmd::parseArgs(const std::string& argsWithoutCommand)
{
	std::string copy(argsWithoutCommand);
	std::vector<std::string> args;
	size_t pos = 0;

	pos = copy.find(' ');
	// GET ONLY <USERNAME> <HOSTNAME> <SERVERNAME>
	while (pos!=std::string::npos && args.size()<3) {
		args.push_back(copy.substr(0, pos));
		copy = copy.substr(pos+1);
		pos = copy.find(' ');
	}
	if (args.size()<3) {
		throw CmdSyntaxErrorException("missing parameters");
	}
	pos = copy.find(':');
	if (pos==std::string::npos) {
		throw CmdSyntaxErrorException("missing semicolon before last parameter");
	}
	copy = copy.substr(pos+1);
	if (copy.empty()) {
		throw CmdSyntaxErrorException("missing last parameter");
	}
	args.push_back(copy);
	return args;
}
