#include "cmd/JoinCmd.hpp"

JoinCmd::JoinCmd(const Server& server)
		:CmdInterface("JOIN", server, false, false, false)
{
}

JoinCmd::~JoinCmd()
{
}

void JoinCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
