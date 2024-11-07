#include "cmd/KickCmd.hpp"
#include "Channel.hpp"
#include "ResponseMsg.hpp"

KickCmd::KickCmd(Server& server)
    : CmdInterface("KICK", server, true, true, false) {
}

//authentication required == true
//if the command should look for a colon ':' == true 
//is colon required == false

KickCmd::~KickCmd() {}


//for /KICK
//<channel> <user> [:reason]

void KickCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
    // Check the required parameters: <channel> <user> //channelName //targetNickname
    if (params.size() < 2) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "KICK <channel> <user>"));
        return;
    }

    const std::string& channelName = params[0];
    const std::string& targetNickname = params[1];

    // Check if the channel exists
    Channel* channel = server.getChannelByName(channelName);
    if (!channel)
    {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), "", "No such channel: " + channelName));
        return;
    }
    if (!channel->isClientInChannel(&requestedFrom)){
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channelName, "You are not inside this channel"));
        return;
    }

    // Verify that the client is an operator in the channel
    if (!channel->isClientOperator(&requestedFrom)) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channelName, "You are not a channel operator"));
        return;
    }

    // Check if the target client is in the channel
    Client* targetClient = server.findClientByNickname(targetNickname);
    if (!targetClient || !channel->isClientInChannel(targetClient)) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_USERNOTINCHANNEL, requestedFrom.getNickname(), channelName, targetNickname + " is not in the channel"));
        return;
    }

    // Perform the kick action
    channel->removeClient(targetClient);
    std::string kickMessage = ":" + requestedFrom.getNickname() + " KICK " + channelName + " " + targetNickname;
    server.sendMessageToChannel(channel, kickMessage);
    // TODO cambia la response CODE
    targetClient->sendMessage(ResponseMsg::genericResponse(ERR_USERNOTINCHANNEL, requestedFrom.getNickname(), channelName, "You have been kicked from " + channelName + " by " + requestedFrom.getNickname()));
}
