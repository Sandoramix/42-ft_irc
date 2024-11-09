#include "cmd/ModeCmd.hpp"

ModeCmd::ModeCmd(Server& server)
		:CmdInterface("MODE", server, true)
{
}

ModeCmd::~ModeCmd()
{
}

void ModeCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	// TODO: implement
}
