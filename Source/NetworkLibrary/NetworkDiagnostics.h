#pragma once

namespace EmberNet
{
	struct NetworkDiagnostics
	{
		unsigned int mySentbytesLastSecond = 0;
		unsigned int myRecievedBytesLastSecond = 0;
		unsigned short myPackageDrop = 0;
		
		struct LatencyData
		{
			float myMilliSeconds = 0;
			unsigned int myPeerID = 0;
		};
		std::vector<LatencyData> myLatencyInMilliSeconds;
	};
}
