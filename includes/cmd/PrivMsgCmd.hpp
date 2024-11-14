#ifndef FT_IRC_PRIVMSGCMD_HPP
#define FT_IRC_PRIVMSGCMD_HPP

#include "CmdInterface.hpp"

class PrivMsgCmd : public CmdInterface {
public:
	PrivMsgCmd(Server& server);
	~PrivMsgCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_PRIVMSGCMD_HPP
