#include "SocketTools/Exceptions.h"
#include "SocketTools/Client.hpp"
#include "ChatShared/ChatMsgHeaders.h"
#include "ChatShared/ChatUtils.h"
#include "ChatClient.hpp"

#include <iostream>
#include <regex>
#include <thread>

inline void runClientChat(const int port, const char* const ip)
{
	auto chatClient = ChatClient(ip, port);
	
	const auto clientReadThread = std::thread(
		[&]() { chatClient.serverReaderRuntime(); }
	);
	
	while (true)
	{
		chatClient.parseUserInput();
	}
}

int main(const int argc, const char* const* argv)
{
	if (argc < 3)
	{
		std::cerr << "No ip and port specified" << std::endl;
		std::cerr << "Ex: 127.0.0.1 8000" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	const auto ip = argv[1];
	const auto port = atoi(argv[2]);
	
	runClientChat(port, ip);
	return 0;
}
