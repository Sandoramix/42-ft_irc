#ifndef FT_IRC_CHANNEL_HPP
#define FT_IRC_CHANNEL_HPP

#include "IRCUtils.hpp"

#include "Server.hpp"
#include "Client.hpp"

class Channel {
private:
	const Server* server;
	std::string name;
	std::string topic;
	/// Maximum number of clients allowed in the channel (0 = unlimited)
	size_t maxClients;

// Other members
private:
	/// Map of clients (key: client socket fd, value: client object)
	ClientsMap clients;
	/// List of invited clients but not yet joined
	std::vector<SocketFd> invitedClients;

	/// List of operator clients
	std::vector<SocketFd> operatorClients;

	/// List of banned clients
	//std::vector<SocketFd> bannedClients;

	std::string password;
	bool isPasswordProtected;

	bool isInviteOnly;
	bool isTopicChangePrivilege;

public:
	Channel(const Server& server, const std::string& name, const std::string& topic);
	~Channel();

	ClientsVector getAllClients() const;
	Client* findClientByNickname(const std::string& nickname);

	bool isClientOperator(Client* client) const;
	bool isClientInChannel(Client* client) const;
	bool isClientInvited(Client* client) const;

	bool inviteClient(Client* client);

	bool addClient(Client* client);
	bool makeOperator(Client* client);
	bool removeOperator(Client* client);
	bool removeClient(Client* client);
	bool removeAllClients();

// GETTERS/SETTERS
public:
	std::string getName() const;
	const std::string& getTopic() const;
	const std::string& getPassword() const;
	bool getPasswordProtected() const;
	size_t getMaxClients() const;
	size_t getClientsSize() const;
	bool getIsInviteOnly() const;
	bool getIsTopicChangeOnlyForOperators() const;
	std::string getClientsNicknames() const;
	bool isPasswordValid(const std::string& passw) const;

	void setName(const std::string& name);
	void setTopic(const std::string& topic);
	void setPassword(const std::string& password);
	void setPasswordProtected(bool passwordProtected);
	void setMaxClients(size_t maxClients);
	void setIsInviteOnly(bool isInviteOnly);
	void setIsTopicChangeOnlyForOperators(bool isTopicChangePrivilege);
};

#endif //FT_IRC_CHANNEL_HPP
