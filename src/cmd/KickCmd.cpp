#include "cmd/KickCmd.hpp"

KickCmd::KickCmd(const Server& server)
		:CmdInterface("KICK", server, true)
{
}

KickCmd::~KickCmd()
{
}

void KickCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
