#include "ChatServer.hpp"
#include "SocketTools/Exceptions.h"
#include <iostream>
#include <regex>

ChatServer::ChatServer(const int port)
{
	m_server = new SocketTools::Server();
	
	try
	{
		m_server->setup(port);
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		std::cerr << "Setup: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	m_server->getNewClientEventHandlers().push_back(
		[&](const SocketTools::Server& server, const int socket)
		{
			this->onNewUserConnects(socket);
		});
	
	m_server->getDisconnectedEventHandlers().push_back(
		[&](const SocketTools::Server& server, const int socket)
		{
			this->onUserDisconnects(socket);
		});
	
	m_server->getClientMsgEventHandlers().push_back(
		[&](
			const SocketTools::Server& server,
			const int socket,
			const unsigned char* const rawBytes,
			const size_t msgLen)
		{
			this->onMsgReceived(rawBytes, msgLen, socket);
		});
}

void ChatServer::waitAndParseAnyActivity()
{
	m_server->waitForAnActivity();
	m_server->parseActivity();
}

/*
** Events handlers.
*/

void ChatServer::onNewUserConnects(const int socket)
{
	// Server side logic.
	{
		std::cout << "New client has connected on fd = " << socket << std::endl;
		
		auto userModel = ChatUserModel();
		userModel.name = STR("Guest" + std::to_string(socket));
		m_users[socket] = userModel;
	}
	
	// Notify the newly connected user.
	{
		const auto successMsg = STR(ChatMsgHeaders::kSendMsg +
			"[Server]: Successfully connected as "
			+ m_users[socket].name);
		sendStr(successMsg, socket);
	}
	
	// Notify everybody else, except the new user.
	{
		const auto newUserMsg = STR(ChatMsgHeaders::kSendMsg +
			m_users[socket].name +
			" has connected.");
		
		broadcastStr(newUserMsg, socket);
	}
}

void ChatServer::onUserDisconnects(const int socket)
{
	const auto userName = m_users[socket].name;
	
	// Server side logic.
	{
		printf("Client has disconnected: fd = %d, name = %s\n",
			socket,
			userName.c_str());
		
		m_users.erase(socket);
	}
	
	// Send notifications to everybody.
	{
		const auto notificationMsg = STR(ChatMsgHeaders::kSendMsg +
			"[" + userName + "]" +
			" has disconnected.");
		broadcastStr(notificationMsg, socket);
	}
}

void ChatServer::onMsgReceived(
	const unsigned char* const msg,
	const size_t msgLen,
	const int socket)
{
	parseRawMsg(msg, msgLen, socket);
}

/*
** Utils.
*/

void ChatServer::sendStr(const std::string str, const int socket)
{
	// Server logging.
	{
		std::cout
			<< "Sending msg = '" << str << "'"
			<< " to socket = " << socket
			<< std::endl;
	}
	
	try
	{
		m_server->sendData(socket, str.c_str(), str.length());
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		fprintf(stderr, "Send: socket = %d: %s\n", socket, e.what());
	}
}

void ChatServer::broadcastStr(const std::string str, const int notToThisSocket)
{
	for (size_t i = 0; i < m_server->getMaxNbOfClients(); i++)
	{
		const auto socket = m_server->getClients()[i];
		
		if (socket > 0 && socket != notToThisSocket)
		{
			sendStr(str, socket);
		}
	}
}

/*
** Msg parsers.
*/

void ChatServer::parseRawMsg(
	const unsigned char* const msg,
	const size_t msgLen,
	const int socket)
{
	const std::string msgStr =
		std::string(reinterpret_cast<const char*>(msg) + 1);
	
	// Server logging.
	{
		printf("[ReceivedMsg][Socket = %d; Name = %s; MsgLen = %zu]: ",
			socket,
			m_users[socket].name.c_str(),
			msgLen);
		std::cout << "'" << msg << "'" << std::endl;
	}
	
	switch (msg[0])
	{
		case ChatMsgHeaders::kSendMsg:
			userSendMsg(msgStr, socket);
			break;
		
		case ChatMsgHeaders::kChangeName:
			tryChangeName(msgStr, socket);
			break;
		
		default:
		{
			const auto invalidMsgWarning = STR(ChatMsgHeaders::kSendMsg +
				"[Server]: invalid msg header.");
			sendStr(invalidMsgWarning, socket);
		}
	}
}

int ChatServer::tryChangeName(const std::string name, const int socket)
{
	std::smatch matches;
	
	// Server logging.
	{
		printf("[ChangeNameAttempt][Socket = %d]: name = '%s'\n",
			socket,
			name.c_str());
	}
	
	auto itMatches = std::regex_search(
		name,
		matches,
		std::regex("^([a-zA-Z0-9]+)$"));
	
	if (!itMatches)
	{
		// Server logging.
		{
			printf("[FailedChangeName][Socket = %d]: name = '%s'\n",
				socket,
				name.c_str());
		}
		
		sendStr(
			STR(ChatMsgHeaders::kSendMsg + name + ": Not a vliad name."),
			socket);
		return -1;
	}
	
	const auto newName = matches[1].str();
	const auto oldName = m_users[socket].name;
	m_users[socket].name = newName;
	
	// Server logging.
	{
		printf("Changed name of %s to %s; socket = %d\n",
			oldName.c_str(),
			newName.c_str(),
			socket);
	}
	
	// Send success notification.
	{
		const auto successMsg = STR(ChatMsgHeaders::kSendMsg +
			"[Server]: Successfully changed name from " +
			oldName + " to " + newName + ".");
		sendStr(successMsg, socket);
	}
	
	// Notify everybody else.
	{
		const auto notificationMsg = STR(ChatMsgHeaders::kSendMsg +
			"[Server]: User " + oldName +
			" has changed his name to " + newName + ".");
		broadcastStr(notificationMsg, socket);
	}
	return 0;
}

void ChatServer::userSendMsg(std::string msg, const int socket)
{
	msg = "[" + m_users[socket].name + "]: " + msg;
	broadcastStr(STR(ChatMsgHeaders::kSendMsg + msg));
}
