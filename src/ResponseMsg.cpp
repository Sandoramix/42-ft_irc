#include <sstream>
#include "ResponseMsg.hpp"

// PRIVATE METHODS ------------------------------------------------------------
std::string ResponseMsg::hostname;

ResponseMsg::ResponseMsg() { }
ResponseMsg::~ResponseMsg() { }
//------------------------------------------------------------------------------

// Pad code with leading zeros (max 3 digits). Example: 1 -> "001"
std::ostream& operator<<(std::ostream& os, const ResponseCode& code)
{
	std::string codeAsString;

	std::stringstream _code_stream;
	size_t _code = code;
	_code_stream << _code;
	codeAsString = _code_stream.str();

	codeAsString.insert(0, 3-codeAsString.size(), '0');
	os << codeAsString;
	return os;
}
//------------------------------------------------------------------------------


std::string ResponseMsg::getDefaultCodeMessage(ResponseCode code)
{
	switch (code) {
	case RPL_WELCOME:
		return "Welcome to the 42Firenze IRC Server@"+hostname;
	case RPL_ENDOFWHO:
		return "End of /WHO list.";
	case RPL_NOTOPIC:
		return "No topic is set";
	case RPL_CHANNELMODEIS:
		return "Channel modes are:";
	case RPL_WHOREPLY:
		return "<User info>";
	case RPL_NAMREPLY:
		return "<User list>";
	case RPL_ENDOFNAMES:
		return "End of /NAMES list.";
	case RPL_BANLIST:
		return "<Ban list>";
	case RPL_ENDOFBANLIST:
		return "End of /BANLIST";
	case ERR_NOSUCHNICK:
		return "No such nick";
	case ERR_NOSUCHCHANNEL:
		return "No such channel";
	case ERR_CANNOTSENDTOCHAN:
		return "Cannot send to channel";
	case ERR_NOTEXTTOSEND:
		return "No text to send";
	case ERR_UNKNOWNCOMMAND:
		return "Unknown command";
	case ERR_ERRONEUSNICKNAME:
		return "Erroneous nickname";
	case ERR_NICKNAMEINUSE:
		return "Nickname is already in use";
	case ERR_NOTONCHANNEL:
		return "You're not on that channel";
	case ERR_NOTREGISTERED:
		return "You have not registered";
	case ERR_NEEDMOREPARAMS:
		return "Not enough parameters";
	case ERR_ALREADYREGISTRED:
		return "You may not reregister";
	case ERR_PASSWDMISMATCH:
		return "Password incorrect";
	case ERR_KEYSET:
		return "Channel key is already set";
	case ERR_CHANNELISFULL:
		return "Channel is full";
	case ERR_UNKNOWNMODE:
		return "Unknown mode";
	case ERR_INVITEONLYCHAN:
		return "Cannot join channel (+i)";
	case ERR_BADCHANNELKEY:
		return "Cannot join channel (+k)";
	case ERR_USERNOTINCHANNEL:
		return "User is not in channel";
	case ERR_CHANOPRIVSNEEDED:
		return "You are not a channel operator";
	default:
		return "Unknown error";
	}
}

//------------------------------------------------------------------------------

void ResponseMsg::setHostname(const std::string& newHostname) { hostname = newHostname; }
std::string ResponseMsg::getHostname() { return hostname.empty() ? "*" : hostname; }

//------------------------------------------------------------------------------

std::string ResponseMsg::genericResponse(ResponseCode code, const std::string& targetNickname)
{
	return genericResponse(code, targetNickname, getDefaultCodeMessage(code));
}

std::string ResponseMsg::genericResponse(ResponseCode code, const std::string& targetNickname, const std::string& customMessage)
{
	std::stringstream ss;

	ss << ":" << getHostname()
	   << code
	   << " " << (targetNickname.empty() ? "*" : targetNickname)
	   << " :" << customMessage;

	debugResponse(ss.str());
	return ss.str();
}

std::string ResponseMsg::errorResponse(ResponseCode code, const std::string& targetNickname, const std::string& commandOrChannelName)
{
	return errorResponse(code, targetNickname, commandOrChannelName, getDefaultCodeMessage(code));
}

std::string ResponseMsg::errorResponse(ResponseCode code, const std::string& targetNickname, const std::string& commandOrChannelName, const std::string& customMessage)
{
	std::stringstream ss;

	ss << ":" << getHostname()
	   << code
	   << " " << (targetNickname.empty() ? "*" : targetNickname)
	   << " " << (commandOrChannelName.empty() ? "*" : commandOrChannelName)
	   << " :" << customMessage;

	debugResponse(ss.str());
	return ss.str();
}

std::string ResponseMsg::noticeResponse(const std::string& senderNickname, const std::string& commandName, const std::string& message)
{
	std::stringstream ss;

	ss << ":" << getHostname()
	   << " " << (senderNickname.empty() ? "*" : senderNickname)
	   << " " << (commandName.empty() ? "*" : commandName)
	   << " :" << message;
	debugResponse(ss.str());
	return ss.str();
}
std::string ResponseMsg::noticeResponse(const std::string& senderNickname, const std::string& targetNickname, const std::string& commandName, const std::string& message)
{
	std::stringstream ss;

	ss << ":" << getHostname()
	   << " " << (senderNickname.empty() ? "*" : senderNickname)
	   << " " << (commandName.empty() ? "*" : commandName)
	   << " " << (targetNickname.empty() ? "*" : targetNickname)
	   << " :" << message;
	debugResponse(ss.str());
	return ss.str();
}

std::string ResponseMsg::welcomeResponse(const Client& client)
{
	std::stringstream ss;

	ss << ":" << getHostname() << RPL_WELCOME << " " << client.getUserInfo() << " :Welcome to this IRC Server";
	debugResponse(ss.str());
	return ss.str();
}

std::string ResponseMsg::modeUpdateResponse(const std::vector<std::string>& params)
{

	std::stringstream ss;

	ss << ":" << getHostname() << " " << "MODE" << (params.empty() ? "" : " ");
	for (size_t i = 0; i<params.size(); i++) {
		if (i>0) { ss << " "; }
		ss << params[i];
	}
	return ss.str();
}

std::string ResponseMsg::replyModeActiveListResponse(const Channel& channel)
{
	std::stringstream ss;
	std::vector<std::string> modeValues;

	ss << channel.getName() << " MODE ";

	int enabledModes = 0;
	ss << "+";
	if (channel.getIsInviteOnly()) {
		ss << "i";
		enabledModes++;
	}
	if (channel.getIsTopicChangeOnlyForOperators()) {
		ss << "t";
		enabledModes++;
	}
	if (channel.getPasswordProtected()) {
		ss << "k";
		modeValues.push_back(channel.getPassword());
		enabledModes++;
	}
	if (channel.getMaxClients()>0) {
		ss << "l";
		std::stringstream limit;
		limit << channel.getMaxClients();
		modeValues.push_back(limit.str());
		enabledModes++;
	}
	if (enabledModes > 0){
		ss << " ";
		for (size_t i = 0; i<modeValues.size(); i++) {
			if (i>0) { ss << " "; }
			ss << modeValues[i];
		}
	}
	return ss.str();
}

std::string ResponseMsg::inviteResponse(const Client& requestedFrom, const Client& invitedClient, const std::string& channelName)
{
	std::stringstream ss;

	ss << ":" << requestedFrom.getNickname() << " INVITE " << invitedClient.getNickname() << " :"+channelName;
	debugResponse(ss.str());
	return ss.str();
}

std::string ResponseMsg::nicknameChangeResponse(const Client& client, const std::string& newNickname)
{
	std::stringstream ss;
	ss << ":" << client.getNickname() << " NICK :" << newNickname;
	return ss.str();
}

std::string ResponseMsg::joinConfirmResponse(const Client& client, const std::string& channelName)
{
	std::stringstream ss;
	ss << ":" << client.getUserInfo() << " JOIN :" << channelName;
	debugResponse(ss.str());
	return ss.str();
}

std::string ResponseMsg::replyNamesListResponse(const Client& client, const Channel& channel, const std::string& userList)
{
	std::stringstream ss;
	const std::string& channelName = channel.getName();

	ss << ":" << getHostname() << " " << RPL_NAMREPLY << " " << client.getNickname() << " = " << (channelName.empty() ? "*" : channelName) << " " << userList;
	debugResponse(ss.str());
	return ss.str();
}
std::string ResponseMsg::replyEndOfNamesResponse(const Client& client, const Channel& channel)
{
	std::stringstream ss;
	const std::string& channelName = channel.getName();

	ss << ":" << getHostname() << " " << RPL_ENDOFNAMES << " " << client.getNickname() << " " << (channelName.empty() ? "*" : channelName) << " :End of /NAMES list.";
	debugResponse(ss.str());
	return ss.str();
}


std::string ResponseMsg::pongResponse(const std::string& token)
{
	std::stringstream ss;
	ss << "PONG :" << token;
	debugResponse(ss.str());
	return ss.str();
}
std::string ResponseMsg::privMsgResponse(const std::string& sender, const std::string& target, const std::string& message)
{
	std::stringstream ss;

	ss << ":" << sender << " PRIVMSG " << target << " :" << message;
	return ss.str();
}
std::string ResponseMsg::userKickedResponse(const std::string& kickerNickname, const std::string& kickedNickname, const std::string& channelName, const std::string& reason)
{
	std::stringstream ss;

	ss << ":" << kickerNickname << " KICK " << channelName << " " << kickedNickname << (reason.empty() ? "" : " :"+reason);
	debugResponse(ss.str());
	return ss.str();
}
std::string ResponseMsg::whoResponse(Client* client, const Channel* channel)
{
	std::stringstream ss;

	ss << ":" << hostname << " " << RPL_WHOREPLY;
	ss << " " << client->getNickname();
	ss << " " << (channel ? channel->getName() : "*");
	ss << " " << client->getUsername();
	ss << " " << client->getHostname();
	ss << " " << hostname;
	bool isTargetOperator = channel!=NULL && channel->isClientOperator(client);
	ss << " " << (isTargetOperator ? "O" : "H");
	ss << " :0";
	debugResponse("TO[" << client->getSocketFd() << "]: " << ss.str());
	return ss.str();
}


