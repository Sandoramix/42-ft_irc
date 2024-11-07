#ifndef FT_IRC_RESPONSEMSG_HPP
#define FT_IRC_RESPONSEMSG_HPP

#include <string>
#include <iostream>
#include "Client.hpp"

/**
 * IRC response messages
 * For more information, see <a href="https://tools.ietf.org/html/rfc1459">RFC 1459</a>
 * or <a href="https://datatracker.ietf.org/doc/html/rfc1459#section-6">here</a>
 */
enum ResponseCode {
	RPL_WELCOME = 1,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	// Error replies
	ERR_NOSUCHCHANNEL = 403,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTRED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_BADCHANNELKEY = 475,
    ERR_USERNOTINCHANNEL = 441,
    ERR_CHANOPRIVSNEEDED = 482,
};


class ResponseMsg {
private:
	static std::string hostname;

	ResponseMsg();
	~ResponseMsg();

public:
	static std::string getDefaultMessage(ResponseCode code);
	static bool isHostnameSet();
	static void setHostname(const std::string& newHostname);

	static std::string genericResponse(ResponseCode code, const std::string& target);
	static std::string genericResponse(ResponseCode code, const std::string& target, const std::string &channelName, const std::string& customMessage);

	static std::string nicknameChangeResponse(const std::string& oldNickname, const std::string& newNickname);
	static std::string joinConfirmResponse(const Client &client, const std::string &channelName);
};

#endif //FT_IRC_RESPONSEMSG_HPP