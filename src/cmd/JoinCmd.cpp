#include "cmd/JoinCmd.hpp"

JoinCmd::JoinCmd(const IRCServer &server): CmdInterface("JOIN", server, false)
{
}

JoinCmd::~JoinCmd()
{
}

void JoinCmd::run(const Client &requestedFrom, const std::vector<std::string> &params)
{
	if (!this->canUserRun(requestedFrom))
	{
		// TODO: send error message
		return;
	}
	// TODO: implement
}
