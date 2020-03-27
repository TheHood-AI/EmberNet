#pragma once
#include <string>
#include <concurrent_queue.h>
#include <functional>

#include "ExportDefines.h"
#include "Packet.h"
#include "SocketDescriptor.h"
#include "NetworkDiagnostics.h"

#define BIND(aFunction, thisPointer) std::bind(&aFunction, thisPointer, std::placeholders::_1, std::placeholders::_2)

namespace EmberNet {

	bool EXPORT StartWSA();

	struct EXPORT CreateParameters
	{
		unsigned short myMillisecondsBetweenPing = 1000;
		unsigned char myPingTriesBeforeDisconnect = 255;
	};

	class CPeer;
	struct SocketDescriptor;

	enum class SendFlags : char
	{
		NoFlag,
		Guaranteed,
		BigPackage
	};

	class EXPORT IPeer
	{
	public:
		IPeer();
		~IPeer();


		bool Startup(const EmberNet::SocketDescriptor& aSocketDescriptor, const CreateParameters& someCreateParameters) const;
		bool Startup(const CreateParameters& someCreateParameters) const;
		void Connect(const std::string& aRemoteAddres, const unsigned short aPort) const;
		bool WorkThroughMessages() const;
		void Shutdown() const;
		unsigned int GetUID() const;
		void Send(const char* someDataToSend, const unsigned int aDataSize, SendFlags aFlag = SendFlags::NoFlag) const;
		void SendTo(const char* someDataToSend, const unsigned int aDataSize, SendFlags aFlag, int aPeerID);
		void SendToAllExcept(const char* someDataToSend, const unsigned int aDataSize, SendFlags aFlag, int aPeerIDToIgnore);
		void MapFunctionToMessage(std::function<void(char*, unsigned int)> aFunction, unsigned char aMessage) const;
		NetworkDiagnostics GetNetworkDiagnostics()const;
	private:
		CPeer* myPeer;
	};
}