#include "cmd/PassCmd.hpp"

PassCmd::PassCmd(const Server& server)
		:CmdInterface("PASS", server, false)
{
}

PassCmd::~PassCmd()
{
}

void PassCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (requestedFrom.getState()>=CS_PASS_SENT) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_ALREADYREGISTRED, requestedFrom.getNickname()));
		debug("Client[" << requestedFrom.getSocketFd() << "] tried to register with the password command but the client is already registered");
		return;
	}
	if (params.empty()) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname()));
		debug("Client[" << requestedFrom.getSocketFd() << "] tried to register with the password command but did not provide any parameters");
		return;
	}
	if (params.size()>1) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "Too many parameters"));
		debug("Client[" << requestedFrom.getSocketFd() << "] tried to register with the password command but provided too many parameters");
		return;
	}
	if (!this->server.isPasswordValid(params[0])){
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_PASSWDMISMATCH, requestedFrom.getNickname()));
		debug("Client[" << requestedFrom.getSocketFd() << "] tried to register with the password command but the password is invalid");
		return;
	}
	requestedFrom.setState(CS_PASS_SENT);
	debug("Client[" << requestedFrom.getSocketFd() << "] registered with the password command");
}

std::vector<std::string> PassCmd::parseArgs(const std::string& argsWithoutCommand)
{
	std::vector<std::string> args;
	args.push_back(argsWithoutCommand);
	return args;
}