#ifndef FT_IRC_INVITECMD_HPP
#define FT_IRC_INVITECMD_HPP

#include "CmdInterface.hpp"

class InviteCmd : public CmdInterface {
public:
	InviteCmd(Server& server);
	~InviteCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_INVITECMD_HPP
