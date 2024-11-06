#ifndef FT_IRC_KICKCMD_HPP
#define FT_IRC_KICKCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class KickCmd : public CmdInterface {
public:
	KickCmd(Server& server);
	~KickCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_KICKCMD_HPP
