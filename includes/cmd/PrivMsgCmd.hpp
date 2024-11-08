#ifndef FT_IRC_PRIVMSGCMD_HPP
#define FT_IRC_PRIVMSGCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class PrivMsgCmd : public CmdInterface {
public:
	PrivMsgCmd(Server& server);
	~PrivMsgCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);
};


#endif //FT_IRC_PRIVMSGCMD_HPP
