#include "cmd/PrivMsgCmd.hpp"

PrivMsgCmd::PrivMsgCmd(Server& server)
		:CmdInterface("PRIVMSG", server, true) { }

PrivMsgCmd::~PrivMsgCmd() { }

void PrivMsgCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	std::string usage = this->commandName + " <target> <message>";
	if (params.size()!=2) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		return;
	}
	const std::string& targetClientOrChannel = params[0];
	const std::string& messageToSend(params[1]);

	if (targetClientOrChannel.empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), "No nick given"));
		return;
	}
	if (messageToSend.empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTEXTTOSEND, requestedFrom.getNickname(), "No text to send"));
		return;
	}
	// IS CHANNEL
	if (IRCUtils::isValidChannelStartingCharacter(targetClientOrChannel)) {
		Channel* channel = server.getChannelByName(targetClientOrChannel);
		if (!channel) {
			requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), targetClientOrChannel));
			return;
		}
		if (!channel->isClientInChannel(&requestedFrom)) {
			requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOTONCHANNEL, requestedFrom.getNickname(), targetClientOrChannel));
			return;
		}
		std::vector<SocketFd> excludeClients(1);
		excludeClients[0] = requestedFrom.getSocketFd();
		this->server.sendMessageToChannel(channel, excludeClients, ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
		debugResponse(ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
		return;
	}
	// IS CLIENT
	Client* targetClient = server.findClientByNickname(targetClientOrChannel, true);
	if (!targetClient || !targetClient->isFullyRegistered()) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), targetClientOrChannel));
		return;
	}

	targetClient->sendMessage(ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
	debugResponse(ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
}