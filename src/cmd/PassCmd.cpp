#include "cmd/PassCmd.hpp"

PassCmd::PassCmd(const Server& server)
		:CmdInterface("MODE", server, true)
{
}

PassCmd::~PassCmd()
{
}

void PassCmd::run(const Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
