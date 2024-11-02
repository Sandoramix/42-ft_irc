#include "Client.hpp"
Client::Client(const Client& other)
		:socketFd(other.socketFd), state(other.state), localBuffer(other.localBuffer), hostname(other.hostname),
		 nickname(other.nickname), username(other.username), password(other.password) { }

Client& Client::operator=(const Client& other)
{
	if (this!=&other) {
		this->socketFd = other.socketFd;
		this->state = other.state;
		this->localBuffer = other.localBuffer;
		this->hostname = other.hostname;
		this->nickname = other.nickname;
		this->username = other.username;
		this->password = other.password;
	}
	return *this;
}
Client::Client()
		:state(CS_UNKNOWN) { }
Client::Client(const int& socketFd)
		:socketFd(socketFd), state(CS_UNKNOWN)
{}

Client::~Client() { }

bool Client::isAuthenticated() const { return (this->state==CS_AUTHENTICATED); }

bool Client::sendMessage(const std::string& message) const
{
	std::string messageToSend = message+"\r\n";
	ssize_t bytesWritten = send(this->getSocketFd(), messageToSend.c_str(), messageToSend.size(), 0);
	if (bytesWritten<0 || (size_t)bytesWritten != messageToSend.size()) {
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
const std::string& Client::getHostname() const { return hostname; }
const std::string& Client::getNickname() const { return nickname; }
const std::string& Client::getUsername() const { return username; }
const std::string& Client::getPassword() const { return password; }

void Client::setSocketFd(const int& socket_fd) { socketFd = socket_fd; }
void Client::setState(const ClientState& state) { this->state = state; }
void Client::setLocalBuffer(const std::string& local_buffer) { this->localBuffer = local_buffer; }
void Client::setHostname(const std::string& hostname) { this->hostname = hostname; }
void Client::setNickname(const std::string& nickname) { this->nickname = nickname; }
void Client::setUsername(const std::string& username) { this->username = username; }
void Client::setPassword(const std::string& password) { this->password = password; }


