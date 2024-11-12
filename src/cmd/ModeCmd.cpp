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
	if (params.empty())
	{
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), ""));
		return;
	}
	bool isTargetAChannel = IRCUtils::isValidChannelName(params[0]);
	if (isTargetAChannel){
		// TODO: implement channel mode
	} else {
		// TODO: implement user mode
	}
	(void)params;
	(void)requestedFrom;
}
