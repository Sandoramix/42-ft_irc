#ifndef FT_IRC_WHOCMD_HPP
#define FT_IRC_WHOCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class WhoCmd : public CmdInterface {
public:
	WhoCmd(Server& server);
	~WhoCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_WHOCMD_HPP
