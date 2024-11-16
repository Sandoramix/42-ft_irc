#ifndef FT_IRC_MODECMD_HPP
#define FT_IRC_MODECMD_HPP

#include "CmdInterface.hpp"

class ModeCmd : public CmdInterface {
public:
	ModeCmd(Server& server);
	~ModeCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);

private:
	enum Mode {
		MODE_INVITEONLY = 'i',
		MODE_TOPICCHANGE = 't',
		MODE_PASSWORD = 'k',
		MODE_OPERATOR = 'o',
		MODE_LIMIT = 'l',
	};

	bool executeViewOnlyMode(Client& requestedFrom, Channel* channel, const std::vector<std::string>& params);
};

#endif //FT_IRC_MODECMD_HPP
