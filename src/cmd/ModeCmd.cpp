#include "cmd/ModeCmd.hpp"

ModeCmd::ModeCmd(Server& server)
		:CmdInterface("MODE", server, true)
{
}

ModeCmd::~ModeCmd()
{
}

void ModeCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (params.empty())
	{
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), ""));
		return;
	}
	bool isTargetAChannel = IRCUtils::isValidChannelName(params[0]);
	if (!isTargetAChannel){
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), params[0]));
		return;
	}
	Channel * channel = server.getChannelByName(params[0]);
	for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it)
	{
		if (*it == "+i")
			channel->setIsInviteOnly(true);
		else if (*it == "-i")
			channel->setIsInviteOnly(false);
		else if (*it == "+t")
			channel->setIsTopicChangePrivilege(true);
		else if (*it == "-t")
			channel->setIsTopicChangePrivilege(false);
		else if (*it == "+k")
			channel->setPasswordProtected(true);
		else if (*it == "-k")
			channel->setPasswordProtected(false);
		else if (*it == "+o"){
			if (++it != params.end() && channel->getClientsNicknames().find(*it))
				channel->makeOperator(server.findClientByNickname(*it));
			else
				requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), channel->getName()));
		}
		else if (*it == "-o")
			if (++it != params.end() && channel->getClientsNicknames().find(*it))
				channel->removeOperator(server.findClientByNickname(*it));
			else
				requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), channel->getName()));
		else if (*it == "+l"){
			std::string tmp = *++it;
			char * end;
			long limit = strtol(end);
			if (it != params.end() && (*it))
				channel->makeOperator(server.findClientByNickname(*it));
			else
				requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), channel->getName()));
		}
		else if (*it == "-l")
			if (++it != params.end() && channel->getClientsNicknames().find(*it))
				channel->removeOperator(server.findClientByNickname(*it));
			else
				requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), channel->getName()));
	}
	(void)params;
	(void)requestedFrom;
}
