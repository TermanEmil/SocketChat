#pragma once

#include "ChatMsgHeaders.h"
#include "SocketTools/SocketIO.hpp"
#include <string>

#define STR(stuff) (std::string() + stuff)

namespace ChatUtils
{
	void sendMsg(SocketTools::SocketIO& socketIO, int socket, std::string msg);
}
