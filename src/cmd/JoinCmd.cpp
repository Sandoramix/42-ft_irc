#include "cmd/JoinCmd.hpp"

JoinCmd::JoinCmd(Server &server)
	: CmdInterface("JOIN", server, true)
{
}

JoinCmd::~JoinCmd()
{
}

void JoinCmd::run(Client &requestedFrom, const std::vector<std::string> &params)
{
	std::vector<std::string> copiedParams(params);

	if (copiedParams.empty())
	{
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "Not enough parameters. Usage: /JOIN <channel> [password]"));
		return;
	}

	// check for atleast 1 arg

	std::string channelName = copiedParams[0];
	if (!IRCUtils::isValidChannelName(channelName))
	{
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), channelName, "Invalid channel name"));
		return;
	}

	std::string possiblePassword = "";
	bool isPasswordProvided = false;

	if (copiedParams.size() > 1)
	{
		possiblePassword = copiedParams[copiedParams.size() - 1];
		if (!IRCUtils::isValidChannelName(possiblePassword))
		{
			isPasswordProvided = true;
			copiedParams.pop_back();
		}
	}
	// TODO: se vogliamo gestire join di piu' canali, modificare questo if.
	if (copiedParams.size() != 1)
	{
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "Invalid number of args"));
		return;
	}
	Channel *channel = server.getChannelByName(channelName);
	// Controlla se il canale esiste già; se no, crealo
	if (channel == NULL)
	{
		channel = server.addChannel(channelName);
		channel->setPasswordProtected(isPasswordProvided);
		channel->setPassword(possiblePassword);
	}
	else
	{
		if (channel->getPasswordProtected() && !channel->isPasswordValid(possiblePassword))
		{
			requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_BADCHANNELKEY, requestedFrom.getNickname(), channel->getName(), "Cannot join channel"));
			// TODO send error msg
			return;
		}
	}

	// Aggiungi l'utente al canale
	channel->addClient(&requestedFrom);

	// Messaggio di conferma JOIN
	this->server.sendMessageToChannel(channel, std::vector<SocketFd>(), ResponseMsg::joinConfirmResponse(requestedFrom, channelName));

	// Invio lista utenti
	std::string userList = channel->getClientsNicknames();
	requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_NAMREPLY, requestedFrom.getNickname(), "= " + channelName, userList));

	// Fine della lista
	requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_ENDOFNAMES, requestedFrom.getNickname(), channelName, "End of /NAMES list."));
}
