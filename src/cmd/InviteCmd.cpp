#include "cmd/InviteCmd.hpp"

InviteCmd::InviteCmd(const Server& server)
		:CmdInterface("INVITE", server, true)
{
}

InviteCmd::~InviteCmd()
{
}

void InviteCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
