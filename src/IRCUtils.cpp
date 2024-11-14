#include "IRCUtils.hpp"

namespace IRCUtils {
	bool isValidChannelStartingCharacter(const std::string& channel)
	{
		return !channel.empty() && (channel[0]=='#' && channel[0]!='&');
	}
	bool isValidChannelName(const std::string& channel)
	{
		if (channel.empty() || channel.size()>50) { return false; }
		if (!isValidChannelStartingCharacter(channel)) { return false; }
		for (size_t i = 1; i<channel.size(); i++) {
			if (!isalnum(channel[i]) && channel[i]!='-' && channel[i]!='_' && channel[i]!='|') { return false; }
		}
		return true;
	}
};