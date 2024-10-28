#include <iostream>

#include "IRCServer.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	try
	{
		IRCServer server(argv[1], argv[2]);
		server.run();
	}
	catch (const IRCServer::BadConfigException &e)
	{
		std::cerr << "Bad configuration: " << e.what() << std::endl;
		return (1);
	} catch (const std::exception &e)
	{
		std::cerr << "Unexpected exception occurred: " << e.what() << std::endl;
		return (1);
	}
	return 0;
}
