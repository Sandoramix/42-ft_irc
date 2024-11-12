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
	if (params.empty() || params.size() > 2) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "Invalid number of arguments. Usage: /TOPIC <channel> [topic]"));
		return;
	}
	const std::string& channelName = params[0];
	Channel * channel = server.getChannelByName(channelName);
	if (channel == NULL) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), channelName));
		return;
    }
	if (params.size() == 1){
		const std::string& topic = channel->getTopic();
		if (topic.empty())
			requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_NOTOPIC, requestedFrom.getNickname(), channel->getName()));
		else
			requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_TOPIC, requestedFrom.getNickname(), channel->getName(), topic));
	}
	if (params.size() == 2){
		if (channel->isClientOperator(&requestedFrom)){
			channel->setTopic(params[1]);
		}
		else {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channel->getName()));
			return;
		}
	}
	return;
}
