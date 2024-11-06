#include "cmd/InviteCmd.hpp"

InviteCmd::InviteCmd(Server& server)
		:CmdInterface("INVITE", server, true, false, false)
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
