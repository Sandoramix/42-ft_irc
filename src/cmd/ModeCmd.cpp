#include "cmd/ModeCmd.hpp"

ModeCmd::ModeCmd(const Server& server)
		:CmdInterface("MODE", server, true)
{
}

ModeCmd::~ModeCmd()
{
}

void ModeCmd::run(const Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
