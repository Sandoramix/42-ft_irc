#include "cmd/ModeCmd.hpp"

ModeCmd::ModeCmd(Server& server)
		:CmdInterface("MODE", server, true) { }

ModeCmd::~ModeCmd() { }

bool ModeCmd::executeViewOnlyMode(Client& requestedFrom, Channel* channel, const std::vector<std::string>& params)
{
	const std::string& mode = params[1];
	if (mode=="+b") {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(RPL_ENDOFBANLIST, requestedFrom.getNickname(), channel->getName()));
		return true;
	}

	return false;
}

void ModeCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	std::string usage = "Usage: " + this->commandName + " <channel> <mode> [param/s]";
	if (params.empty()) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		return;
	}

	const std::string& channelName = params[0];
	bool isTargetAChannel = IRCUtils::isValidChannelName(channelName);
	if (!isTargetAChannel) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), params[0]));
		return;
	}
	Channel* channel = server.getChannelByName(channelName);
	if (!channel) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOSUCHCHANNEL, requestedFrom.getNickname(), channelName));
		return;
	}
	if (!channel->isClientInChannel(&requestedFrom)) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NOTONCHANNEL, requestedFrom.getNickname(), channelName));
		return;
	}

	// Show channel modes and exit
	if (params.size()==1) {
		requestedFrom.sendMessage(ResponseMsg::replyModeActiveListResponse(*channel));
		return;
	}

	// Special cases which can be executed even if the client is not a channel operator (not fully implemented).
	if (this->executeViewOnlyMode(requestedFrom, channel, params)) {
		return;
	}

	// Every mode below can be executed only by channel operators
	if (!channel->isClientOperator(&requestedFrom)) {
		requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_CHANOPRIVSNEEDED, requestedFrom.getNickname(), channelName));
		return;
	}

	const std::string& mode = params[1];
	/// TRUE=`+`; FALSE=`-`
	if (mode.size()<=1) {
		requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
		return;
	}

	bool modeSwitch = mode[0]=='+';
	size_t lastUsedParam = 1;
	for (size_t i = 1; i<mode.size(); i++) {
		switch (mode[i]) {
		case MODE_INVITEONLY: {
			channel->setIsInviteOnly(modeSwitch);
			break;
		}
		case MODE_TOPICCHANGE: {
			channel->setIsTopicChangeOnlyForOperators(modeSwitch);
			break;
		}
		case MODE_PASSWORD: {
			if (modeSwitch && channel->getPasswordProtected()) {
				requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_KEYSET, requestedFrom.getNickname(), channelName));
				return;
			}
			if (!modeSwitch) {
				channel->setPasswordProtected(false);
				channel->setPassword("");
			}
			else {
				lastUsedParam++;
				if (lastUsedParam>=params.size()) {
					requestedFrom
							.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
					return;
				}
				std::string password = params[lastUsedParam];
				if (password.empty()) {
					requestedFrom
							.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
					return;
				}
				channel->setPasswordProtected(true);
				channel->setPassword(password);
			}
			break;
		}
		case MODE_OPERATOR: {
			lastUsedParam++;
			if (lastUsedParam>=params.size()) {
				requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
				return;
			}
			const std::string& nickname = params[lastUsedParam];
			if (nickname.empty()) {
				requestedFrom.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
				return;
			}
			Client* client = channel->findClientByNickname(nickname);
			if (!client) {
				requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_USERNOTINCHANNEL, requestedFrom.getNickname(), channelName));
				return;
			}
			if (modeSwitch) {
				channel->makeOperator(client);
			}
			else {
				channel->removeOperator(client);
			}
			break;
		}
		case MODE_LIMIT: {
			if (modeSwitch) {
				lastUsedParam++;
				if (lastUsedParam>=params.size()) {
					requestedFrom
							.sendMessage(ResponseMsg::genericResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), usage));
					return;
				}
				const std::string& limitStr = params[lastUsedParam];
				char* end;
				long limit = strtol(limitStr.c_str(), &end, 10);
				if (end==limitStr.c_str() || *end!='\0' || limit<0 || limit<(long)channel->getClientsSize()) {
					requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_NEEDMOREPARAMS, requestedFrom.getNickname(), channelName, "Invalid limit"));
					return;
				}
				channel->setMaxClients(limit);
			}
			else {
				channel->setMaxClients(0);
			}
			break;
		}
		default: {
			std::stringstream ss;
			ss << "(" << mode[i] << ")";
			requestedFrom.sendMessage(ResponseMsg::errorResponse(ERR_UNKNOWNMODE, requestedFrom.getNickname(), channelName, ss.str()));
			return;
		}
		}
	}
	this->server.sendMessageToChannel(channel, std::vector<SocketFd>(), ResponseMsg::modeUpdateResponse(params));
}


