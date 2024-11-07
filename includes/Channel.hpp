#ifndef FT_IRC_CHANNEL_HPP
#define FT_IRC_CHANNEL_HPP

#include <map>
#include <string>

class Channel;
#include "Server.hpp"
#include "Client.hpp"

class Channel {
// Assignable from constructor
private:
	const Server* server; // TODO: remove this?
	bool isPrivateChannel;
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

	std::vector<SocketFd> operatorClients;

	/// List of banned clients
	//std::vector<SocketFd> bannedClients;

	/// TODO: add settings for `MODE` command
	/// password for the channel. If empty, the channel is public
	std::string password;
	bool isPasswordProtected;

	bool isInviteOnly;
	bool isTopicReadOnly;



public:
	Channel(const Server& server, const std::string& name, const std::string& topic, bool isPrivate);
	~Channel();

public:
// CLIENT RELATED METHODS

	ClientsVector getAllClients() const;

	bool isClientOperator(Client* client) const;
	bool isClientInChannel(Client* client) const;
	bool isClientInvited(Client* client) const;

	bool inviteClient(Client* client);


	bool addClient(Client* client);
	bool removeClient(Client* client);
	bool removeAllClients();

// GETTERS/SETTERS
public:
	bool getIsPrivateChannel() const;
	const std::string& getName() const;
	const std::string& getTopic() const;
	const std::string& getPassword() const;
	bool getPasswordProtected() const;
	size_t getMaxClients() const;
	bool getIsInviteOnly() const;
	bool getIsTopicReadOnly() const;
	std::string getClientsNicknames() const;
	bool isPasswordValid(const std::string &passw) const;

	void setIsPrivateChannel(bool isPrivate);
	void setName(const std::string& name);
	void setTopic(const std::string& topic);
	void setPassword(const std::string& password);
	void setPasswordProtected(bool passwordProtected);
	void setMaxClients(size_t maxClients);
	void setIsInviteOnly(bool isInviteOnly);
	void setIsTopicReadOnly(bool isTopicReadOnly);
};

#endif //FT_IRC_CHANNEL_HPP