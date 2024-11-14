#ifndef FT_IRC_PINGCMD_HPP
#define FT_IRC_PINGCMD_HPP

#include "CmdInterface.hpp"

class PingCmd : public CmdInterface {
public:
	PingCmd(Server& server);
	~PingCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_PINGCMD_HPP
