#ifndef FT_IRC_PASSCMD_HPP
#define FT_IRC_PASSCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class PassCmd : public CmdInterface {
public:
	PassCmd(Server& server);
	~PassCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);

	std::vector<std::string> parseArgs(const std::string& argsWithoutCommand);
};

#endif //FT_IRC_PASSCMD_HPP
