#include "cmd/InviteCmd.hpp"

InviteCmd::InviteCmd(Server& server)
		:CmdInterface("INVITE", server, true)
{
}

InviteCmd::~InviteCmd()
{
}

void InviteCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (params.size() !=2){
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "You must type: <nickname> <channel>"));
	}
	
	std::string channelName = params[1];
    Channel * channel = server.getChannelByName(channelName);
    if (channel == NULL) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), channelName));
		return;
    }

    std::string nickname = params[0];
    Client * user = server.findClientByNickname(nickname, true);
    if (user == NULL) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHNICK, requestedFrom.getNickname(), nickname));
		return;
    }

	if (channel->isClientInChannel(user)){
		return;
	}

    // Verifica se il mittente è operatore del canale
    if (!channel->isClientOperator(&requestedFrom)) {
        requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channelName));
		return;
    }

    // Invia messaggio di invito all'utente invitato
    channel->inviteClient(user);
    user->sendMessage(ResponseMsg::inviteResponse(requestedFrom, *user, channelName));
}
