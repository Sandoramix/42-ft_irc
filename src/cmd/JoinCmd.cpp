#include "cmd/JoinCmd.hpp"

JoinCmd::JoinCmd(Server& server)
		:CmdInterface("JOIN", server, true) { }

JoinCmd::~JoinCmd() { }

void JoinCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	std::string usage = "Usage: " + this->commandName + " <channel> [password]";
	std::vector<std::string> copiedParams(params);

	if (copiedParams.empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		return;
	}

	std::string channelName = copiedParams[0];
	if (!IRCUtils::isValidChannelName(channelName)) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), channelName, "Invalid channel name"));
		return;
	}

	std::string possiblePassword;
	bool isPasswordProvided = false;

	if (copiedParams.size()>1) {
		possiblePassword = copiedParams[copiedParams.size()-1];
		if (!IRCUtils::isValidChannelName(possiblePassword)) {
			isPasswordProvided = true;
			copiedParams.pop_back();
		}
	}
	if (copiedParams.size()!=1) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		return;
	}
	Channel* channel = server.getChannelByName(channelName);
	// Controlla se il canale esiste già; se no, crealo
	if (channel==NULL) {
		channel = server.addChannel(channelName);
		channel->setPasswordProtected(isPasswordProvided);
		channel->setPassword(possiblePassword);
	}
	else {
		if (channel->getPasswordProtected() && !channel->isPasswordValid(possiblePassword)) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_BADCHANNELKEY, requestedFrom.getNickname(), channel->getName()));
			return;
		}
		if (channel->getIsInviteOnly() && !channel->isClientInvited(&requestedFrom)) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_INVITEONLYCHAN, requestedFrom.getNickname(), channel->getName()));
			return;
		}
		if (channel->getMaxClients()!=0 && channel->getMaxClients()==channel->getClientsSize()) {
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_CHANNELISFULL, requestedFrom.getNickname(), channel->getName()));
			return;
		}
	}

	// Aggiungi l'utente al canale
	channel->addClient(&requestedFrom);

	// Messaggio di conferma JOIN
	this->server.sendMessageToChannel(channel, std::vector<SocketFd>(), ResponseMsg::joinConfirmResponse(requestedFrom, channelName));

	// Invio lista utenti
	std::string userList = channel->getClientsNicknames();
	requestedFrom.sendMessage(ResponseMsg::replyNamesListResponse(requestedFrom, *channel, userList));

	// Fine della lista
	requestedFrom.sendMessage(ResponseMsg::replyEndOfNamesResponse(requestedFrom, *channel));
}
