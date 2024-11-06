#ifndef FT_IRC_RESPONSEMSG_HPP
#define FT_IRC_RESPONSEMSG_HPP

#include <string>
#include <iostream>

/**
 * IRC response messages
 * For more information, see <a href="https://tools.ietf.org/html/rfc1459">RFC 1459</a>
 * or <a href="https://datatracker.ietf.org/doc/html/rfc1459#section-6">here</a>
 */
enum ResponseCode {
	RPL_WELCOME = 1,
	// Error replies
	ERR_UNKNOWNCOMMAND = 421,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NOTREGISTRED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTRED = 462,
	ERR_PASSWDMISMATCH = 464,
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
	static std::string genericResponse(ResponseCode code, const std::string& target, const std::string& customMessage);

	static std::string nicknameChangeResponse(const std::string& oldNickname, const std::string& newNickname);
};

#endif //FT_IRC_RESPONSEMSG_HPP
