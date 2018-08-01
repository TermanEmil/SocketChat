#include "SocketTools/Exceptions.h"
#include "ChatServer.hpp"
#include <iostream>

static inline void runChatServer(const int port)
{
	auto chatServer = ChatServer(port);
	while (true)
	{
		chatServer.waitAndParseAnyActivity();
	}
}

int main(const int argc, const char* const* argv)
{
	if (argc == 1)
	{
		std::cerr << "No port specified" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	const auto port = atoi(argv[1]);
	runChatServer(port);
	return 0;
}
