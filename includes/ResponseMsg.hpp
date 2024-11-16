#ifndef FT_IRC_RESPONSEMSG_HPP
#define FT_IRC_RESPONSEMSG_HPP

#include "IRCUtils.hpp"

#include "Client.hpp"

/**
 * IRC response messages
 * For more information, see <a href="https://tools.ietf.org/html/rfc1459">RFC 1459</a>
 * or <a href="https://datatracker.ietf.org/doc/html/rfc1459#section-6">here</a>
 */
enum ResponseCode {
	RPL_WELCOME = 1,
	RPL_ENDOFWHO = 315,
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_CHANNELMODEIS = 324,

	RPL_WHOREPLY = 352,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	RPL_BANLIST = 367,
	RPL_ENDOFBANLIST = 368,

	// Error replies
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL = 403,
	ERR_CANNOTSENDTOCHAN = 404,
	ERR_NOTEXTTOSEND = 412,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NOTONCHANNEL = 442,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTRED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_KEYSET = 467,
	ERR_CHANNELISFULL = 471,
	ERR_UNKNOWNMODE = 472,
	ERR_INVITEONLYCHAN = 473,
	ERR_BADCHANNELKEY = 475,
	ERR_USERNOTINCHANNEL = 441,
	ERR_CHANOPRIVSNEEDED = 482,
};

class ResponseMsg {
private:
	static std::string hostname;
public:
	static const std::string& getHostname();

	ResponseMsg();
	~ResponseMsg();

private:
	static std::stringstream& generateResponseCommonPart(std::stringstream& ss, ResponseCode code, const std::string& target);

public:
	static std::string getDefaultMessage(ResponseCode code);
	static bool isHostnameSet();
	static void setHostname(const std::string& newHostname);

	static std::string genericResponse(ResponseCode code, const std::string& target, const std::string& channelName);
	static std::string genericResponse(ResponseCode code, const std::string& target, const std::string& channelName, const std::string& customMessage);

	static std::string genericCommandResponse(const std::string& commandName, const std::vector<std::string>& params);

	static std::string inviteResponse(const Client& requestedFrom, const Client& invitedClient, const std::string& channelName);
	static std::string nicknameChangeResponse(const Client& client, const std::string& newNickname);
	static std::string joinConfirmResponse(const Client& client, const std::string& channelName);
	static std::string pongResponse(const std::string& token);
	static std::string privMsgResponse(const std::string& sender, const std::string& target, const std::string& message);
	static std::string userKickedResponse(const std::string& kickerNickname, const std::string& kickedNickname, const std::string& channelName, const std::string& reason);
	static std::string whoResponse(Client* client, const Channel* channel);
};

#endif //FT_IRC_RESPONSEMSG_HPP
