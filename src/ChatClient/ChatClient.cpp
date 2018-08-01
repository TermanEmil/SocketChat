#include "ChatClient.hpp"
#include "ChatShared/ChatUtils.h"
#include "SocketTools/Exceptions.h"
#include <iostream>

ChatClient::ChatClient(const char* ip, int port)
{
	m_client = new SocketTools::Client();
	
	try
	{
		printf("Trying to connect on %s:%d\n", ip, port);
		m_client->setup(ip, port);
		printf("Successfully connected on %s:%d\n", ip, port);
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		std::cerr << "Setup: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

ChatClient::~ChatClient()
{
	if (m_client)
	{
		delete m_client;
	}
}

/*
** Public methods
*/

void ChatClient::parseUserInput()
{
	std::string userInput;
	std::getline(std::cin, userInput);
	std::cin.clear();

	if (std::cin.eof())
	{
		std::cout << "End of input" << std::endl;
		exit(EXIT_SUCCESS);
	}

	if (errno == EINTR)
	{
		errno = 0;
		std::cin.clear();
		return;
	}

	if (parseChangeNameCmd(userInput)) {}
	else
	{
		sendMsg(userInput);
	}
}

void ChatClient::serverReaderRuntime()
{
	while (true)
	{
		const auto readResult = m_client->receiveData(m_client->getSocket());
		
		if (readResult == -1 || readResult == 0)
		{
			std::cerr << "Disconnected from server." << std::endl;
			exit(EXIT_SUCCESS);
		}

		const auto out = m_client->getReceiveBuf();
		switch (out[0])
		{
			case ChatMsgHeaders::kSendMsg:
				std::cout << (out + 1) << std::endl;
				break;
			default:
			{
				std::cerr << "invalid msg received from server." << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
}

bool ChatClient::parseChangeNameCmd(const std::string userInput)
{
	static const auto setNameStr = "set name to ";
	
	if (strncmp(setNameStr, userInput.c_str(), strlen(setNameStr)) == 0)
	{
		changeName(userInput.c_str() + strlen(setNameStr));
		return true;
	}
	return false;
}

void ChatClient::sendMsg(const std::string msg)
{
	try
	{
		ChatUtils::sendMsg(*m_client, m_client->getSocket(), msg);
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		std::cerr << "Send: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ChatClient::changeName(const std::string newName)
{
	const auto setNewNameCmd = STR(ChatMsgHeaders::kChangeName + newName);
	
	try
	{
		m_client->sendData(
			m_client->getSocket(),
			setNewNameCmd.c_str(),
			setNewNameCmd.length());
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		std::cerr << "Send: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}
