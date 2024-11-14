#ifndef FT_IRC_USERCMD_HPP
#define FT_IRC_USERCMD_HPP

#include "CmdInterface.hpp"

class UserCmd : public CmdInterface {
public:
	UserCmd(Server& server);
	~UserCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_USERCMD_HPP
