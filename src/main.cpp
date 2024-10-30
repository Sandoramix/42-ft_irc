#include <iostream>
#include <csignal>

#include "Server.hpp"

void handleSigInt(int sig) { if (sig==SIGINT) { SERVER_RUNNING = false; }}

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
		std::cerr << "Bad configuration: " << e.what() << std::endl;
		return (1);
	}
	catch (const std::exception& e) {
		std::cerr << "Unexpected exception occurred: " << e.what() << std::endl;
		return (1);
	}
	return 0;
}
