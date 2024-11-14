#include <sstream>
#include "cmd/WhoCmd.hpp"

WhoCmd::WhoCmd(Server& server)
		:CmdInterface("WHO", server, true)
{
}

WhoCmd::~WhoCmd()
{
}

void WhoCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (params.size()!=1 || params[0].empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "WHO <target>"));
		return;
	}

	ClientsVector clientsToSend;
	Channel* channel = NULL;

	const std::string& target = params[0];
	bool isTargetAChannel = IRCUtils::isValidChannelStartingCharacter(target);

	if (!isTargetAChannel) {
		Client* targetClient = server.findClientByNickname(target, true);
		if (!targetClient) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), target));
			return;
		}
		clientsToSend.push_back(targetClient);
	}
	else {
		channel = server.getChannelByName(target);
		if (!channel) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), target));
			return;
		}
		if (!channel->isClientInChannel(&requestedFrom)) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTONCHANNEL, requestedFrom.getNickname(), target));
			return;
		}
//		if (!channel->isClientOperator(&requestedFrom)) {
//			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), target));
//			return;
//		}
		const ClientsVector& clients = channel->getAllClients();
		clientsToSend.insert(clientsToSend.end(), clients.begin(), clients.end());
	}
	for (ClientsVector::iterator it = clientsToSend.begin(); it!=clientsToSend.end(); ++it) {
		requestedFrom.sendMessage(ResponseMsg::whoResponse(*it, channel));
	}
	requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_ENDOFWHO, requestedFrom.getNickname(), channel ? channel->getName() : "*"));
}
