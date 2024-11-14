#ifndef FT_IRC_KICKCMD_HPP
#define FT_IRC_KICKCMD_HPP

#include "CmdInterface.hpp"

class KickCmd : public CmdInterface {
public:
	KickCmd(Server& server);
	~KickCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif // FT_IRC_KICKCMD_HPP
