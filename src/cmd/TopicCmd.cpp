#include "cmd/TopicCmd.hpp"

TopicCmd::TopicCmd(const Server& server)
		:CmdInterface("TOPIC", server, true)
{
}

TopicCmd::~TopicCmd()
{
}

void TopicCmd::run(const Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
