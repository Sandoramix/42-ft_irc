#include "cmd/PingCmd.hpp"

PingCmd::PingCmd(Server& server)
		:CmdInterface("PING", server, false, true, true)
{
}

PingCmd::~PingCmd()
{
}

void PingCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	if (params.size()!=1) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), "", "PING <message>"));
		return;
	}
	requestedFrom.sendMessage(ResponseMsg::pongResponse(params[0]));
}
