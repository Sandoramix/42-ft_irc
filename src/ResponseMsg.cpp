#include <sstream>
#include "ResponseMsg.hpp"

// PRIVATE METHODS ------------------------------------------------------------
std::string ResponseMsg::hostname;

ResponseMsg::ResponseMsg() { }
ResponseMsg::~ResponseMsg() { }

std::string ResponseMsg::getDefaultMessage(ResponseCode code)
{
	switch (code) {
	case RPL_WELCOME:
		return "Welcome to the 42Firenze IRC Server@"+hostname;
	case RPL_NOTOPIC:
		return "No topic is set";
	case RPL_NAMREPLY:
		return "<User list>";
	case RPL_ENDOFNAMES:
		return "End of /NAMES list.";
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
	case ERR_BADCHANNELKEY:
		return "Cannot join channel";
	case ERR_USERNOTINCHANNEL:
		return "User is not in channel";
	case ERR_CHANOPRIVSNEEDED:
		return "You are not a channel operator";
	default:
		return "Unknown error";
	}
}



// PUBLIC METHODS -------------------------------------------------------------

void ResponseMsg::setHostname(const std::string& newHostname) { hostname = newHostname; }

bool ResponseMsg::isHostnameSet()
{
	if (!ResponseMsg::hostname.empty()) {
		return true;
	}
	std::cerr << "Error: hostname is empty" << std::endl;
	return false;
}

std::string ResponseMsg::genericResponse(ResponseCode code, const std::string& target, const std::string& channelName)
{
	return genericResponse(code, target, channelName, getDefaultMessage(code));
}

std::string ResponseMsg::genericResponse(ResponseCode code, const std::string& target, const std::string& channelName, const std::string& customMessage)
{
	std::string host = isHostnameSet() ? hostname : "*";
	std::stringstream ss;

	std::stringstream codeStream;
	codeStream << code;
	int codeSize = codeStream.str().size();
	codeStream.str("");
	while (codeStream.str().size()+codeSize<3) {
		codeStream << "0";
	}
	codeStream << code;

	std::string codeAsString = codeStream.str();

	ss << ":" << host << " " << codeAsString << " " << (target.empty() ? "*" : target) << (channelName.empty() ? "" : " "+channelName) << " :" << customMessage;
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
	ss << ":" << client.getNickname() << "!" << client.getUsername() << "@" << client.getHostname() << " JOIN :" << channelName;
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

	ss << ":" << kickerNickname << " KICK " << channelName << " " << kickedNickname << (reason.empty() ? "" : " :" + reason);
	debugResponse(ss.str());
	return ss.str();
}
