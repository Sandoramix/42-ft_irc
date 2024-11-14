#ifndef JOINCMD_HPP
#define JOINCMD_HPP

#include "CmdInterface.hpp"

class JoinCmd : public CmdInterface {
public:
	JoinCmd(Server& server);
	~JoinCmd();

	void run(Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //JOINCMD_HPP
