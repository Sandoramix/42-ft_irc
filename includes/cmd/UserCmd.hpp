#ifndef FT_IRC_USERCMD_HPP
#define FT_IRC_USERCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class UserCmd : public CmdInterface {
public:
	UserCmd(const Server& server);
	~UserCmd();

public:
	virtual void run(Client& requestedFrom, const std::vector<std::string>& params);

};

#endif //FT_IRC_USERCMD_HPP
