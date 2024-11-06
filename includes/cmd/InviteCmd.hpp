#ifndef FT_IRC_INVITECMD_HPP
#define FT_IRC_INVITECMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class InviteCmd : public CmdInterface {
public:
	InviteCmd(Server& server);
	~InviteCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_INVITECMD_HPP
