#ifndef JOINCMD_HPP
#define JOINCMD_HPP

#include <string>

#include "CmdInterface.hpp"

class JoinCmd: public CmdInterface
{
	public:
		JoinCmd(const IRCServer &server);
		~JoinCmd();

		virtual void run(const Client &requestedFrom, const std::vector<std::string> &params);
};

#endif //JOINCMD_HPP
