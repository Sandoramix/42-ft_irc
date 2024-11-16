#include "cmd/UserCmd.hpp"

UserCmd::UserCmd(Server& server)
		:CmdInterface("USER", server, false) { }

UserCmd::~UserCmd() { }

/**
 * <br>
 * @brief The USER command is used to identify the user to the server. It is sent by the client to the server when the client connects to the server.
 * <br><br>
 *
 * Syntax: USER \<username> \<mode> \<unused> \<realname>
 *
 *
 * @note Exactly 4 parameters are required.
 *
 * <fieldset>
 * 	<legend>Parameters</legend>
 * 	<ul>
 * 		<li><b>username</b>: the username of the user</li>
 * 		<li><b>mode</b>: mode of the user. It wont be used in this implementation. Usually it is set to "0" by the client.</li>
 * 		<li><b>unused</b>: unused parameter. It is ignored by the server. Usually it is set to "0" or "*" by the client.</li>
 * 		<li><b>realname</b>: the realname of the user. It is used to set the realname of the user.</li>
 * 	</ul>
 * </fieldset>
 */
void UserCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	std::string usage = "Usage: " + this->commandName + " <username> <mode> <unused> <realname>";
	if (requestedFrom.getState()<CS_PASS_SENT) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NOTREGISTERED, requestedFrom.getNickname()));
		return;
	}
	if (requestedFrom.isUserCmdSent()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_ALREADYREGISTRED, requestedFrom.getNickname()));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but the client is already registered");
		return;
	}
	if (params.size()!=4) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		debugError("Client[" << requestedFrom.getSocketFd() << "] tried to register but invalid number of parameters was provided");
		return;
	}

	std::string username = params[0];
	requestedFrom.setUsername(username);
//	std::string mode = params[1];
//	std::string unused = params[2];
	std::string realname = params[3];
	requestedFrom.setRealName(realname);

	requestedFrom.setIsUserCmdSent(true);
	if (requestedFrom.isUserCmdSent() && requestedFrom.isNickCmdSent() && !requestedFrom.isFullyRegistered()) {
		requestedFrom.setState(CS_ISFULLY_REGISTERED);
		requestedFrom.sendMessage(ResponseMsg::genericResponse(RPL_WELCOME, requestedFrom.getNickname()));
	}
}
