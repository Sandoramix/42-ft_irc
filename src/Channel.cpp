#include "Channel.hpp"

// CONSTRUCTOR
Channel::Channel(const Server& server, const std::string& name, const std::string& topic)
		:server(&server), name(name), topic(topic), maxClients(0), password(), isPasswordProtected(false), isInviteOnly(false), isTopicChangePrivilege(false) { (void)this->server; }

// DESTRUCTOR
Channel::~Channel()
{
	this->removeAllClients();
	this->invitedClients.clear();
	this->operatorClients.clear();
	this->clients.clear();
}

//------------------------------------------------------------------------------
bool Channel::isClientInChannel(Client* client) const
{
	if (!client) { return false; }
	return this->clients.find(client->getSocketFd())!=this->clients.end();
}

bool Channel::isClientOperator(Client* client) const
{
	if (!this->isClientInChannel(client)) {
		debug("Client[" << client->getSocketFd() << "] is not in channel[" << this->name << "]");
		return false;
	}
	if (std::find(this->operatorClients.begin(), this->operatorClients.end(), client->getSocketFd())==this->operatorClients.end()) {
		debug("Client[" << client->getSocketFd() << "] is not an operator in channel[" << this->name << "]");
		return false;
	}
	return true;
}

bool Channel::isClientInvited(Client* client) const
{
	if (!client) { return false; }
	return std::find(this->invitedClients.begin(), this->invitedClients.end(), client->getSocketFd())!=this->invitedClients.end();
}

ClientsVector Channel::getAllClients() const
{
	ClientsVector result;
	for (ClientsMap::const_iterator it = this->clients.begin(); it!=this->clients.end(); ++it) {
		if (it->second==NULL) { continue; }
		result.push_back(it->second);
	}
	return result;
}


Client* Channel::findClientByNickname(const std::string& nickname)
{
	for (ClientsMap::iterator it = this->clients.begin(); it!=this->clients.end(); ++it) {
		if (IRCUtils::caseInsensitiveStringCompare(it->second->getNickname(), nickname)){
			return it->second;
		}
	}
	return NULL;
}

bool Channel::removeAllClients()
{
	this->clients.clear();
	this->operatorClients.clear();
	return true;
}


//------------------------------------------------------------------------------

bool Channel::addClient(Client* client)
{
	if (!client) { return false; }

	if (this->maxClients>0 && this->clients.size()>=this->maxClients) {
		debug("Client[" << client->getSocketFd() << "] tried to join channel[" << this->name << "] but the channel is full");
		return false;
	}

	if (this->isInviteOnly) {
		const bool wasInvited = this->isClientInvited(client);
		if (!wasInvited) {
			debugWarning("Client[" << client->getSocketFd() << "] tried to join channel[" << this->name << "] but it is invite only");
			return false;
		}
		this->invitedClients.erase(std::find(this->invitedClients.begin(), this->invitedClients.end(), client->getSocketFd()));
	}

	if (this->operatorClients.empty()) { this->operatorClients.push_back(client->getSocketFd()); }

	this->clients[client->getSocketFd()] = client;
	return true;
}

bool Channel::removeOperator(Client* client)
{
	if (!client) { return false; }

	if (!this->isClientOperator(client)) { return false; }

	this->operatorClients.erase(std::find(this->operatorClients.begin(), this->operatorClients.end(), client->getSocketFd()));

	if (this->operatorClients.empty() && !this->clients.empty()) {
		debug("Client[" << client->getSocketFd() << "] left channel as last operator.");
		Client* newOperator = this->clients.begin()->second;
		debug("Promoting client[" << newOperator->getSocketFd() << "] to operator");
		this->operatorClients.push_back(newOperator->getSocketFd());
	}

	return true;
}

bool Channel::makeOperator(Client* client)
{
	if (!client) { return false; }
	this->operatorClients.push_back(client->getSocketFd());
	return true;
}

bool Channel::removeClient(Client* client)
{
	if (!this->isClientInChannel(client)) { return false; }

	this->clients.erase(client->getSocketFd());

	removeOperator(client);
	const std::vector<SocketFd>::iterator& it = std::find(this->invitedClients.begin(), this->invitedClients.end(), client->getSocketFd());
	if (it!=this->invitedClients.end()) {
		this->invitedClients.erase(it);
	}

	return true;
}

bool Channel::inviteClient(Client* client)
{
	if (!client) {
		return false;
	}
	if (this->isClientInChannel(client)) {
		debug("Client[" << client->getSocketFd() << "] is already in channel[" << this->name << "]");
		return false;
	}
	if (this->maxClients>0 && this->clients.size()>=this->maxClients) {
		debug("Client[" << client->getSocketFd() << "] tried to invite client[" << client->getSocketFd() << "] but the channel is full");
		return false;
	}
	std::vector<SocketFd>::const_iterator it = std::find(this->invitedClients.begin(), this->invitedClients.end(), client->getSocketFd());
	if (it!=this->invitedClients.end()) {
		debug("Client[" << client->getSocketFd() << "] is already invited in channel[" << this->name << "]");
		return false;
	}
	this->invitedClients.push_back(client->getSocketFd());
	return true;
}



// GETTERS/SETTERS ------------------------------------------------------------

std::string Channel::getName() const { return this->name.empty() ? "*" : this->name; }
const std::string& Channel::getTopic() const { return this->topic; }
const std::string& Channel::getPassword() const { return this->password; }
bool Channel::getPasswordProtected() const { return this->isPasswordProtected; }
size_t Channel::getMaxClients() const { return this->maxClients; }
size_t Channel::getClientsSize() const { return this->clients.size(); }
bool Channel::getIsInviteOnly() const { return this->isInviteOnly; }
bool Channel::getIsTopicChangeOnlyForOperators() const { return this->isTopicChangePrivilege; }
std::string Channel::getClientsNicknames() const
{
	std::string clientsNicknames;
	for (ClientsMap::const_iterator it = this->clients.begin(); it!=this->clients.end(); ++it) {
		if (it!=this->clients.begin())
			clientsNicknames += " ";
		clientsNicknames += it->second->getNickname();
	}
	return clientsNicknames;
}
bool Channel::isPasswordValid(const std::string& passw) const
{
	return this->password==passw;
}

void Channel::setName(const std::string& name) { this->name = name; }
void Channel::setTopic(const std::string& topic)
{
	// TODO: check if this is needed
	if (this->isTopicChangePrivilege) {
		debug("Channel[" << this->name << "] is read only. Cannot set topic.");
		return;
	}
	this->topic = topic;
}
void Channel::setPassword(const std::string& password) { this->password = password; }
void Channel::setPasswordProtected(bool passwordProtected) { this->isPasswordProtected = passwordProtected; }
void Channel::setMaxClients(size_t maxClients) { this->maxClients = maxClients; }
void Channel::setIsInviteOnly(bool isInviteOnly) { this->isInviteOnly = isInviteOnly; }
void Channel::setIsTopicChangeOnlyForOperators(bool isTopicChangePrivilege) { this->isTopicChangePrivilege = isTopicChangePrivilege; }
