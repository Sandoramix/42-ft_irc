#ifndef FT_IRC_NICKCMD_HPP
#define FT_IRC_NICKCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class NickCmd : public CmdInterface {
public:
	NickCmd(Server& server);
	~NickCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_NICKCMD_HPP
