#include "cmd/JoinCmd.hpp"

JoinCmd::JoinCmd(const Server& server)
		:CmdInterface("JOIN", server, false)
{
}

JoinCmd::~JoinCmd()
{
}

void JoinCmd::run(const Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
