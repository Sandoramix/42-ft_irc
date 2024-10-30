#ifndef JOINCMD_HPP
#define JOINCMD_HPP

#include <string>
#include <vector>
#include "CmdInterface.hpp"

class JoinCmd : public CmdInterface {
public:
	JoinCmd(const Server& server);
	~JoinCmd();

public:
	virtual void run(const Client& requestedFrom, const std::vector<std::string>& params);
};

#endif //JOINCMD_HPP
