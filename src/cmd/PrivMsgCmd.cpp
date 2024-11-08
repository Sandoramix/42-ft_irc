#include "cmd/PrivMsgCmd.hpp"

PrivMsgCmd::PrivMsgCmd(Server& server)
		:CmdInterface("PRIVMSG", server, true, true, false)
{
}

PrivMsgCmd::~PrivMsgCmd()
{
}

void PrivMsgCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (params.size()!=2) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "PRIVMSG <target> <message>"));
		return;
	}
	std::string targetClientOrChannel = params[0];
	std::string messageToSend(params[1]);
	if (targetClientOrChannel.empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), "", "No nick given"));
		return;
	}
	if (messageToSend.empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTEXTTOSEND, requestedFrom.getNickname(), "", "No text to send"));
		return;
	}
	// IS CHANNEL
	if (targetClientOrChannel[0]=='#') {
		Channel* channel = server.getChannelByName(targetClientOrChannel);
		if (!channel) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), targetClientOrChannel));
			return;
		}
		if (!channel->isClientInChannel(&requestedFrom)) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), targetClientOrChannel, "You are not a channel operator"));
			return;
		}
		this->server.sendMessageToChannel(channel, std::vector<SocketFd>(requestedFrom.getSocketFd()), ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
		debugResponse(ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
		return;
	}
	// IS CLIENT
	else {
		Client* targetClient = server.findClientByNickname(targetClientOrChannel);
		if (!targetClient) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), targetClientOrChannel));
			return;
		}
		if (!targetClient->isFullyRegistered()) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTERED, requestedFrom.getNickname(), targetClientOrChannel, "Client is not fully registered yet"));
			return;
		}
		targetClient->sendMessage(ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
		debugResponse(ResponseMsg::privMsgResponse(requestedFrom.getNickname(), targetClientOrChannel, messageToSend));
	}
}