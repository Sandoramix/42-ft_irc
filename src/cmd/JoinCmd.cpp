#include "cmd/JoinCmd.hpp"

JoinCmd::JoinCmd(Server& server)
		:CmdInterface("JOIN", server, false, false, false)
{
}

JoinCmd::~JoinCmd()
{
}

void JoinCmd::run(Client& requestedFrom, const std::vector<std::string>& params)
{
	(void)params;
	(void)requestedFrom;
	if (!this->canUserRun(requestedFrom)) {
		// TODO: send error message
		return;
	}
	// Controlla se il canale esiste già; se no, crealo
    if (server.channels.find(channelName) == channels.end()) {
        channels[channelName] = Channel{channelName};
    }
    
    // Aggiungi l'utente al canale
    channels[channelName].users.push_back(user);

    // Messaggio di conferma JOIN
    std::string joinMsg = ":" + user.nickname + " JOIN :" + channelName + "\r\n";
    send(user.socket, joinMsg.c_str(), joinMsg.size(), 0);

    // Invio lista utenti
    std::string userList = ":server 353 " + user.nickname + " = " + channelName + " :";
    for (const auto& u : channels[channelName].users) {
        userList += u.nickname + " ";
    }
    userList += "\r\n";
    send(user.socket, userList.c_str(), userList.size(), 0);

    // Fine della lista
    std::string endListMsg = ":server 366 " + user.nickname + " " + channelName + " :End of /NAMES list.\r\n";
    send(user.socket, endListMsg.c_str(), endListMsg.size(), 0);
}
