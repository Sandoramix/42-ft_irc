#include "cmd/NickCmd.hpp"

NickCmd::NickCmd(const Server& server)
		:CmdInterface("NICK", server, false)
{
}

NickCmd::~NickCmd()
{
}

void NickCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (requestedFrom.getState() < CS_PASS_SENT) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTRED, requestedFrom.getNickname()));
		return;
	}
	const std::string& newNickname = params[0];
	if (newNickname.empty() || !isalnum(newNickname[0])) {
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to change nickname to invalid nickname");
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_ERRONEUSNICKNAME, requestedFrom.getNickname()));
		return;
	}
	const std::string& oldNickname = requestedFrom.getNickname();
	requestedFrom.setNickname(params[0]);
	Client* foundClient = this->server.findClientByNickname(newNickname);
	if (foundClient != NULL && foundClient->getSocketFd() != requestedFrom.getSocketFd()) {
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to change nickname to already used nickname");
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NICKNAMEINUSE, requestedFrom.getNickname()));
		return;
	}
	this->server.notifyClientOfNicknameChange(requestedFrom, oldNickname);
}
