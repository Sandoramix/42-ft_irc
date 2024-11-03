#include "cmd/UserCmd.hpp"

UserCmd::UserCmd(const Server& server)
		:CmdInterface("MODE", server, true)
{
}

UserCmd::~UserCmd()
{
}

void UserCmd::run(const Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
