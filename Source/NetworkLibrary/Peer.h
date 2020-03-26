#pragma once

#include <string>
#include "CommonNetworkIncludes.h"
#include <vector>
#include <concurrent_queue.h>
#include "Packet.h"
#include "ExportDefines.h"
#include "SocketDescriptor.h"
#include <unordered_map>
#include <functional>
#include "NetMessage.h"
#include "NetworkDiagnostics.h"
#include "Timer.h"
#include <set>
#include "Socket.h"

#include "NetBigMessage.h"
#include <mutex>
#include <concurrent_priority_queue.h>
#include "Packet.h"

namespace EmberNet
{
	enum class SendFlags : char;

	struct CreateParameters;
	struct Packet;
	class CPeer
	{
	public:
		CPeer() = default;
		~CPeer() ;

		bool Startup(const SocketDescriptor& aSocketDescriptor, const CreateParameters& someCreateParameters) ;
		bool Startup(const CreateParameters& someCreateParameters);
		void Connect(const std::string& aRemoteAddres, const unsigned short aPort) ;
		bool WorkThroughMessages();
		void Shutdown();
		unsigned int GetUID() const;
		void Send(const char* someDataToSend, const unsigned int aDataSize, SendFlags aFlag) ;
		void SendTo(const char* someDataToSend, const unsigned int aDataSize, SendFlags aFlag, int aPeerID);
		void SendToAllExcept(const char* someDataToSend, const unsigned int aDataSize, SendFlags aFlag, int aPeerIDToIgnore);
		void MapFunctionToMessage(std::function<void(char*, unsigned int)> aFunction, unsigned char aMessage);
		NetworkDiagnostics GetNetworkDiagnostics()const;

	private:
		void ListeningThread();
		void PingThread();
		void UpdateThread();
		void GuaranteedMessagesThread();
		void SendGuaranteedMessage(const char* someData, const unsigned int aDataSize, const SOCKADDR& aAddres);
		void SendBigPackage(const char* someData, const unsigned int aDataSize, const SOCKADDR& aAddres);
		void Send(const SOCKET& aSocket, const char* someData, unsigned int aDataSize, int aFlag, const SOCKADDR& aAddres);

		struct PeerInfo
		{
			SOCKADDR myAddress;
			unsigned int myUID = 0;
			unsigned char myPingTries = 0;
			CU::CStopWatch myStopWatch;
		};
		
		struct GuaranteedMessage
		{
			CGuaranteedMessage myMessage;
			unsigned short myResendTries = 0;
			SOCKADDR mySendTo; 
		};

		struct BigPacket
		{
			std::vector<CNetBigPackageChunk> myChunks;
			std::set<unsigned int> myRecievedChunk;
		};

		std::unordered_map<int, BigPacket> myBigBackages;
		std::unordered_map<unsigned char, std::function<void(char*, unsigned int)>> myMappedFunctions;
		std::set<unsigned int> myFinishedBigPackages;
		std::vector<GuaranteedMessage> myGuaranteedMessages;
		CSocket mySocket;
		std::vector<PeerInfo> myConnectedAddreses;
		std::set<std::pair<unsigned short, unsigned int>> myRecievedGuaranteedMesssages;
		concurrency::concurrent_priority_queue<Packet, std::greater<>> myRecievedPackets;

		std::mutex myMutex;

		unsigned int myGUID = 0;
		unsigned short myTimeBetweenPings = 1000;
		unsigned char myTriesBeforeDisconnecting = 5;
		bool myShouldShutDown = false;
		unsigned int myByteSentThisSecond = 0;
		unsigned int myByteSentLastSecond = 0;

		unsigned int myByteRecievedThisSecond = 0;
		unsigned int myByteRecievedLastSecond = 0;

		unsigned int myGuaranteedMessagesSent = 0;
		unsigned int myDroppedMessages = 0;

		float myLatency = 0;
	};
}
