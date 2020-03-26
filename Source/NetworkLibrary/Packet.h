#pragma once
#include "CommonNetworkIncludes.h"
#include "NetMessage.h"

namespace EmberNet
{
	struct Packet
	{
		char data[512];
		int size = 0;
		SOCKADDR sender;
		EPriority myPriority = EPriority::Low;

		bool operator>(const Packet& aOther) const
		{
			return this->myPriority > aOther.myPriority;
		}
	};

	typedef std::unique_ptr<Packet> PacketPtr;
	
}