#include "cmd/PingCmd.hpp"

PingCmd::PingCmd(Server& server)
		:CmdInterface("PING", server, false) { }

PingCmd::~PingCmd() { }

void PingCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	std::string usage = "Usage: " + this->commandName + " <message>";
	if (params.size()!=1) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		return;
	}
	requestedFrom.sendMessage(ResponseMsg::pongResponse(params[0]));
}
