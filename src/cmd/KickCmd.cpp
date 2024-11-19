#include "cmd/KickCmd.hpp"
#include "Channel.hpp"
#include "ResponseMsg.hpp"

KickCmd::KickCmd(Server& server)
		:CmdInterface("KICK", server, true) { }

KickCmd::~KickCmd() { }

/**
 * @brief Usage: KICK \<channel> \<user> [:reason]
 * @param requestedFrom client that is trying to run the command
 * @param params parameters of the command
 */
void KickCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	std::string usage = "Usage: " + this->commandName + " <channel> <user> [reason]";
	// Check the required parameters: <channel> <user> [reason]
	if (params.size()<2 || params.size()>3) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		return;
	}

	const std::string& channelName = params[0];
	const std::string& targetNickname = params[1];
	const std::string& reasonMsg = params.size()==3 ? params[2] : "";

	// Check if the channel exists
	Channel* channel = server.getChannelByName(channelName);
	if (!channel) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), channelName, "No such channel: "+channelName));
		return;
	}
	if (!channel->isClientInChannel(&requestedFrom)) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channelName, "You are not inside this channel"));
		return;
	}

	// Verify that the client is an operator in the channel
	if (!channel->isClientOperator(&requestedFrom)) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channelName));
		return;
	}

	// Check if the target client is in the channel
	Client* targetClient = server.findClientByNickname(targetNickname, true);
	if (!targetClient || !channel->isClientInChannel(targetClient)) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_USERNOTINCHANNEL, requestedFrom.getNickname(), channelName, targetNickname+" is not in the channel"));
		return;
	}

	// Prevent the operator from kicking themselves
    if (&requestedFrom == targetClient) {
        requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_OPERATOR_CANT_KICK_HIMSELF, requestedFrom.getNickname(), channelName, "Operators cannot kick themselves"));
        return;
    }

	// Perform the kick action
	server.sendMessageToChannel(channel, std::vector<SocketFd>(), ResponseMsg::userKickedResponse(requestedFrom.getNickname(), targetClient->getNickname(), channelName, reasonMsg));
	channel->removeClient(targetClient);
}
