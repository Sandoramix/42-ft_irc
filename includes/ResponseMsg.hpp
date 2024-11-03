#ifndef FT_IRC_RESPONSEMSG_HPP
#define FT_IRC_RESPONSEMSG_HPP

#include <string>
#include <iostream>

/**
 * IRC response messages
 * For more information, see <a href="https://tools.ietf.org/html/rfc1459">RFC 1459</a>
 * or <a href="https://www.irchelp.org/irchelp/rfc/chapter6.html">here</a>
 */
enum ResponseCode {

	// Error replies
	ERR_UNKNOWNCOMMAND = 421,
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

	static std::string errorResponse(ResponseCode code, const std::string& target);
	static std::string errorResponse(ResponseCode code, const std::string& target, const std::string& customMessage);

};

#endif //FT_IRC_RESPONSEMSG_HPP
