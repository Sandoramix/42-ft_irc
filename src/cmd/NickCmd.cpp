#include "cmd/NickCmd.hpp"

NickCmd::NickCmd(Server& server)
		:CmdInterface("NICK", server, false, false, false)
{
}

NickCmd::~NickCmd()
{
}

void NickCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (requestedFrom.getState() < CS_PASS_SENT) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTERED, requestedFrom.getNickname()));
		return;
	}
	const std::string& newNickname = params[0];
	if (newNickname.empty() || !isalnum(newNickname[0])) {
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to change nickname to invalid nickname");
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_ERRONEUSNICKNAME, requestedFrom.getNickname()));
		return;
	}
	const std::string& oldNickname = requestedFrom.getNickname();
	Client* foundClient = this->server.findClientByNickname(newNickname);
	if (foundClient != NULL && foundClient->getSocketFd() != requestedFrom.getSocketFd()) {
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to change nickname to already used nickname");
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NICKNAMEINUSE, requestedFrom.getNickname()));
		return;
	}
	if (foundClient){
		debugInfo("Found client [" << foundClient->getSocketFd() << "] Nickname: " << foundClient->getNickname());
	}
	requestedFrom.setIsNickCmdSent(true);
	requestedFrom.setNickname(params[0]);
	if (requestedFrom.getIsUserCmdSent() && requestedFrom.getIsNickCmdSent()){
		requestedFrom.setState(CS_ISFULLY_REGISTERED);
	}
	this->server.notifyClientOfNicknameChange(requestedFrom, oldNickname);
}
