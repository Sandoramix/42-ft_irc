#include "cmd/TopicCmd.hpp"

TopicCmd::TopicCmd(Server& server)
		:CmdInterface("TOPIC", server, true)
{
}

TopicCmd::~TopicCmd()
{
}

void TopicCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	Channel * channel = server.getChannelByName(params[0]);
	if (channel == NULL) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), channel->getName()));
		return;
    }
	if (channel->getIsPrivateChannel() && !(channel->isClientInChannel(&requestedFrom))) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTONCHANNEL, requestedFrom.getNickname(), channel->getName()));
		return;
    }
	if (params.size() == 1){
		std::string topic = channel->getTopic();
		if (topic == "")
			requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_NOTOPIC, requestedFrom.getNickname(), channel->getName()));
		else
			requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_TOPIC, requestedFrom.getNickname(), channel->getName(), topic));
	}
	if (params.size() == 2){
		if (channel->isClientOperator(&requestedFrom))
			channel->setTopic(params[1]);
		else
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channel->getName()));
	}
	else if (params.size() < 1){
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), ""));
	}
	else {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_UNKNOWNCOMMAND, requestedFrom.getNickname(), ""));
	}
	return;
}
