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
	bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 ) {
		std::string str1Cpy( str1 );
		std::string str2Cpy( str2 );

		std::transform(str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower);
		std::transform(str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower);

		return str1Cpy == str2Cpy;
	}
};