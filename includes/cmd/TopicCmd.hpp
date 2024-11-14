#ifndef FT_IRC_TOPICCMD_HPP
#define FT_IRC_TOPICCMD_HPP

#include "CmdInterface.hpp"

class TopicCmd : public CmdInterface {
public:
	TopicCmd(Server& server);
	~TopicCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //FT_IRC_TOPICCMD_HPP
