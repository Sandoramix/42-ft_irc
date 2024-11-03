#include "cmd/UserCmd.hpp"

UserCmd::UserCmd(const Server& server)
		:CmdInterface("USER", server, false)
{
}

UserCmd::~UserCmd()
{
}

void UserCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// TODO: implement
}
