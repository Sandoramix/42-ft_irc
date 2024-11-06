#include "cmd/PassCmd.hpp"

PassCmd::PassCmd(Server& server)
		:CmdInterface("PASS", server, false, false, false)
{
}

PassCmd::~PassCmd()
{
}

void PassCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (requestedFrom.getState()>=CS_PASS_SENT) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_ALREADYREGISTRED, requestedFrom.getNickname()));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but the client is already registered");
		return;
	}
	if (params.empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname()));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but did not provide any parameters");
		return;
	}
	if (params.size()>1) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "Too many parameters"));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but provided too many parameters");
		return;
	}
	if (!this->server.isPasswordValid(params[0])){
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_PASSWDMISMATCH, requestedFrom.getNickname()));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but the password is invalid");
		return;
	}
	requestedFrom.setState(CS_PASS_SENT);
	debugSuccess("Client[" << requestedFrom.getSocketFd() << "] authenticated with correct password");
}

std::vector<std::string> PassCmd::parseArgs(const std::string& argsWithoutCommand)
{
	std::vector<std::string> args;
	if (!argsWithoutCommand.empty()) {
		args.push_back(argsWithoutCommand);
	}
	return args;
}