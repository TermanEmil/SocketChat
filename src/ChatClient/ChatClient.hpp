#pragma once

#include <string>
#include "SocketTools/Client.hpp"

class ChatClient
{
public:
	ChatClient(const char* ip, int port);
	~ChatClient();
	
	void serverReaderRuntime();
	void parseUserInput();
	
private:
	SocketTools::Client* m_client = nullptr;
	
	bool parseChangeNameCmd(std::string userInput);
	void sendMsg(std::string msg);
	void changeName(std::string newName);
};
