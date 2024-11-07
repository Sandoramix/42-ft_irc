#include "cmd/TopicCmd.hpp"

TopicCmd::TopicCmd(Server& server)
		:CmdInterface("TOPIC", server, true, false, false)
{
}

TopicCmd::~TopicCmd()
{
}

void TopicCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	// TODO: implement
}
