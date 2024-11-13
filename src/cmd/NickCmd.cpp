#include "cmd/NickCmd.hpp"

NickCmd::NickCmd(Server& server)
		:CmdInterface("NICK", server, false)
{
}

NickCmd::~NickCmd()
{
}

void NickCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	const int maxNicknameLength = 9;
	if (requestedFrom.getState()<CS_PASS_SENT) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTERED, requestedFrom.getNickname(), ""));
		return;
	}
	std::string newNickname = params[0];

	if (newNickname.empty()) {
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to change nickname to invalid nickname");
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_ERRONEUSNICKNAME, requestedFrom.getNickname(), ""));
		return;
	}
	if (newNickname.size()>maxNicknameLength) {
		newNickname = newNickname.substr(0, maxNicknameLength);
		debugInfo("Client[" << requestedFrom.getSocketFd() << "] tried to change nickname to too long nickname. Nickname truncated to " << newNickname);
	}
	if (newNickname!="*") {
		for (size_t i = 0; i<newNickname.size(); i++) {
			if (!isalnum(newNickname[i]) && newNickname[i]!='-' && newNickname[i]!='_') {
				debugError("Client[" << requestedFrom.getSocketFd() << "] tried to set invalid nickname. Nickname=" << newNickname);
				requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_ERRONEUSNICKNAME, requestedFrom.getNickname(), ""));
				return;
			}
		}
	}

	Client* foundClient = this->server.findClientByNickname(newNickname, true);
	if (foundClient && foundClient->getSocketFd()!=requestedFrom.getSocketFd()) {
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to change nickname to already used nickname");
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NICKNAMEINUSE, requestedFrom.getNickname(), ""));
		return;
	}

	if (!requestedFrom.getNickname().empty()) {
		this->server.notifyClientNicknameChangeToOthers(requestedFrom, newNickname);
	}

	requestedFrom.setNickname(newNickname);

	requestedFrom.setIsNickCmdSent(true);
	if (requestedFrom.getIsUserCmdSent() && requestedFrom.getIsNickCmdSent() && !requestedFrom.isFullyRegistered()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_WELCOME, requestedFrom.getNickname(), ""));
		requestedFrom.setState(CS_ISFULLY_REGISTERED);
	}
}
