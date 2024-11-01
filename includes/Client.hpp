#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>
#include <vector>

#include "IRCUtils.hpp"
#include "cmd/CmdInterface.hpp"

typedef std::vector<CmdInterface *> ClientCommandsQueue;

enum ClientState {
	CS_AUTHENTICATING,
	CS_AUTHENTICATED,
	CS_DISCONNECTED
	//... ?
};

class Client {
private:
	int socketFd;
	ClientState state;
	std::string localBuffer;


	std::string hostname;
	std::string nickname;
	std::string username;
	std::string password;
//...

public:
	Client(const int& socketFd);
	~Client();

	bool isAuthenticated() const;

/// GETTERS/SETTERS
public:
	const int& getSocketFd() const;
	const ClientState& getState() const;
	const std::string& getLocalBuffer() const;
	const std::string& getHostname() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getPassword() const;

	void setSocketFd(const int& socket_fd);
	void setState(const ClientState& state);
	void setLocalBuffer(const std::string& local_buffer);
	void setHostname(const std::string& hostname);
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username);
	void setPassword(const std::string& password);
};

#endif //CLIENT_HPP
