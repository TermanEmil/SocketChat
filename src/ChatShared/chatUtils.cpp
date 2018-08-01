#include "ChatUtils.h"

void ChatUtils::sendMsg(
	SocketTools::SocketIO& socketIO,
	const int socket,
	std::string msg)
{
	msg = ChatMsgHeaders::kSendMsg + msg;
	socketIO.sendData(socket, msg.c_str(), msg.length());
}
