#include <iostream>
#include <csignal>

#include "Server.hpp"

void handleSigInt(int sig)
{
	if (sig==SIGINT) {
		std::cout << "Interrupt signal received. Exiting..." << std::endl;
		SERVER_RUNNING = false;
	}
}

bool SERVER_RUNNING = false;

int main(int argc, char* argv[])
{
	if (argc!=3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	try {
		Server server("0.0.0.0", argv[1], argv[2]);
		signal(SIGINT, handleSigInt);
		server.run();
	}
	catch (const Server::BadConfigException& e) {
		std::cerr << RED << "Bad configuration: " << e.what() << RESET << std::endl;
		return (1);
	}
	catch (Server::ServerException& e) {
		std::cerr << RED << "Server error: " << e.what() << RESET << std::endl;
		return (1);
	}
	catch (const std::exception& e) {
		std::cerr << MAGENTA << "Unexpected exception occurred: " << e.what() << RESET << std::endl;
		return (1);
	}
	return 0;
}
