#pragma once
#include <string>

#define LISTEN_TO_ALL_IP "0.0.0.0"

namespace EmberNet
{
	struct SocketDescriptor
	{
		unsigned short myPort = 0;
		std::string myIPToRecieveFrom = LISTEN_TO_ALL_IP;
	};
}