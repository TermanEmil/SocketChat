#pragma once

#include "SocketTools/Server.hpp"
#include "ChatShared/ChatMsgHeaders.h"
#include "ChatShared/ChatUtils.h"
#include "ChatUserModel.h"

#include <map>

class ChatServer
{
public:
	ChatServer(int port);
	
	void waitAndParseAnyActivity();
	
private:
	SocketTools::Server* m_server;
	std::map<int, ChatUserModel> m_users;
	
	void onNewUserConnects(int socket);
	void onUserDisconnects(int socket);
	void onMsgReceived(const unsigned char* msg, size_t msgLen, int socket);
	
	void sendStr(std::string str, int socket);
	void broadcastStr(std::string str, int notToThisSocket = -1);
	
	void parseRawMsg(const unsigned char* msg, size_t msgLen, int socket);
	int tryChangeName(std::string name, int socket);
	void userSendMsg(std::string msg, int socket);
};
