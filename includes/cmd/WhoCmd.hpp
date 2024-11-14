#ifndef FT_IRC_WHOCMD_HPP
#define FT_IRC_WHOCMD_HPP

#include "CmdInterface.hpp"

class WhoCmd : public CmdInterface {
public:
	WhoCmd(Server& server);
	~WhoCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_WHOCMD_HPP
