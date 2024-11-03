#include "cmd/NickCmd.hpp"

NickCmd::NickCmd(const Server& server)
		:CmdInterface("MODE", server, true)
{
}

NickCmd::~NickCmd()
{
}

void NickCmd::run(const Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
