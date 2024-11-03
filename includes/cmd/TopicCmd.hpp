#ifndef FT_IRC_TOPICCMD_HPP
#define FT_IRC_TOPICCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class TopicCmd : public CmdInterface {
public:
	TopicCmd(const Server& server);
	~TopicCmd();

public:
	virtual void run(const Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_TOPICCMD_HPP
