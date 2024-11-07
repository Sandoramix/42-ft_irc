#ifndef FT_IRC_PINGCMD_HPP
#define FT_IRC_PINGCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class PingCmd : public CmdInterface {
public:
	PingCmd(Server& server);
	~PingCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_PINGCMD_HPP
