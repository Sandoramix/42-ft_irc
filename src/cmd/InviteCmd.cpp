#include "cmd/InviteCmd.hpp"

InviteCmd::InviteCmd(Server& server)
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
	// TODO: implement
}
