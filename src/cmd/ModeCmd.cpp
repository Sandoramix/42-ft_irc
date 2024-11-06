#include "cmd/ModeCmd.hpp"

ModeCmd::ModeCmd(const Server& server)
		:CmdInterface("MODE", server, true, false, false)
{
}

ModeCmd::~ModeCmd()
{
}

void ModeCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
