#ifndef FT_IRC_NICKCMD_HPP
#define FT_IRC_NICKCMD_HPP

#include "CmdInterface.hpp"

class NickCmd : public CmdInterface {
public:
	NickCmd(Server& server);
	~NickCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_NICKCMD_HPP
