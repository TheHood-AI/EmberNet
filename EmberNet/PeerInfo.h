#pragma once

#include "CommonNetworkIncludes.h"


	struct EXPORT PeerInfo
	{
		SOCKADDR myAddress;
		unsigned int myUID = 0;
	};
