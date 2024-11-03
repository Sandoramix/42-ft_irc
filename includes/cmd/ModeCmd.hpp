#ifndef FT_IRC_MODECMD_HPP
#define FT_IRC_MODECMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class ModeCmd : public CmdInterface {
public:
	ModeCmd(const Server& server);
	~ModeCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_MODECMD_HPP
