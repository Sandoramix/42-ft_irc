#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>
#include <vector>
#include <map>

#include "IRCUtils.hpp"

class Client;
typedef std::map<SocketFd, Client*> ClientsMap;
typedef std::vector<Client*> ClientsVector;

#include "IRCUtils.hpp"
#include "Channel.hpp"
#include "cmd/CmdInterface.hpp"


typedef std::vector<CmdInterface *> ClientCommandsQueue;

enum ClientState {
	CS_CONNECTED,
	CS_PASS_SENT,
	CS_ISFULLY_REGISTERED,
	CS_DISCONNECTED
	//... ?
};

class Client {
private:
	int socketFd;
	ClientState state;
	std::string localBuffer;

	std::string nickname;

	std::string username;
	std::string hostname;
	std::string realName;

	bool isUserCmdSent;
	bool isNickCmdSent;

private:
	Client(const Client&);
	Client& operator=(const Client&);
public:
	Client();
	Client(const int& socketFd);
	~Client();

	bool isFullyRegistered() const;

public:
	bool sendMessage(const std::string& message) const;

	std::string getUserInfo() const;

/// GETTERS/SETTERS
public:
	const int& getSocketFd() const;
	const ClientState& getState() const;
	const std::string& getLocalBuffer() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getHostname() const;
	const std::string& getRealName() const;
	const bool& getIsUserCmdSent() const;
	const bool& getIsNickCmdSent() const;

	void setSocketFd(const int& socket_fd);
	void setState(const ClientState& state);
	void setLocalBuffer(const std::string& local_buffer);
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username);
	void setHostname(const std::string& hostname);
	void setRealName(const std::string& realName);
	void setIsUserCmdSent(const bool& isUserCmdSent);
	void setIsNickCmdSent(const bool& isNickCmdSent);
};

#endif //CLIENT_HPP
