#include "Client.hpp"

Client::Client(const Client& other)
		:socketFd(other.socketFd), state(other.state), localBuffer(other.localBuffer), nickname(other.nickname), username(other.username), hostname(other.hostname), serverName(other.serverName),
		 realName(other.realName), isUserCmdSent(other.isUserCmdSent), isNickCmdSent(other.isNickCmdSent) { }

Client& Client::operator=(const Client& other)
{
	if (this!=&other) {
		this->socketFd = other.socketFd;
		this->state = other.state;
		this->localBuffer = other.localBuffer;
		this->hostname = other.hostname;
		this->nickname = other.nickname;
		this->username = other.username;
	}
	return *this;
}
Client::Client()
		:state(CS_CONNECTED), localBuffer(), nickname(), username(), hostname(), serverName(), realName(), isUserCmdSent(false), isNickCmdSent(false) { }
Client::Client(const int& socketFd)
		:socketFd(socketFd), state(CS_CONNECTED), localBuffer(), nickname(), username(), hostname(), serverName(), realName(), isUserCmdSent(false), isNickCmdSent(false) { }

Client::~Client() { }

bool Client::isAuthenticated() const { return (this->state==CS_ISFULLY_REGISTERED); }

bool Client::sendMessage(const std::string& message) const
{
	std::string messageToSend = message+"\r\n";
	ssize_t bytesWritten = send(this->getSocketFd(), messageToSend.c_str(), messageToSend.size(), 0);
	if (bytesWritten<0 || (size_t)bytesWritten!=messageToSend.size()) {
		debug("Error while sending message to client[" << this->getSocketFd() << "]. Message=" << message);
//		std::cerr << "Error occurred while sending message to client[" << client->getSocketFd() << "]" << std::endl;
		return false;
	}
	return true;
}
// GETTERS/SETTERS ------------------------------------------------------------

const int& Client::getSocketFd() const { return socketFd; }
const ClientState& Client::getState() const { return state; }
const std::string& Client::getLocalBuffer() const { return localBuffer; }
const std::string& Client::getNickname() const { return nickname; }
const std::string& Client::getUsername() const { return username; }
const std::string& Client::getHostname() const { return hostname; }
const std::string& Client::getServerName() const { return serverName; }
const std::string& Client::getRealName() const { return realName; }
const bool& Client::getIsUserCmdSent() const { return isUserCmdSent; }
const bool& Client::getIsNickCmdSent() const { return isNickCmdSent; }

void Client::setSocketFd(const int& socket_fd) { socketFd = socket_fd; }
void Client::setState(const ClientState& state) { this->state = state; }
void Client::setLocalBuffer(const std::string& local_buffer) { this->localBuffer = local_buffer; }
void Client::setHostname(const std::string& hostname) { this->hostname = hostname; }
void Client::setNickname(const std::string& nickname) { this->nickname = nickname; }
void Client::setUsername(const std::string& username) { this->username = username; }
void Client::setServerName(const std::string& serverName) { this->serverName = serverName; }
void Client::setRealName(const std::string& realName) { this->realName = realName; }
void Client::setIsUserCmdSent(const bool& isUserCmdSent) { this->isUserCmdSent = isUserCmdSent; }
void Client::setIsNickCmdSent(const bool& isNickCmdSent) { this->isNickCmdSent = isNickCmdSent; }

