#include "PeerWrapper.h"
#include <cassert>
#include <utility>
#include "Peer.h"
#include "HelperMacros.h"

void EmberNet::StartWSA()
{
	WSADATA wsaData;

	const WORD wVersionRequested = MAKEWORD(2, 2);

	const int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		EMBER_NET_LOG("WSAStartup failed with error: %d\n", err);
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		EMBER_NET_LOG("Could not find a usable version of Windsock.dll\n");
		WSACleanup();
	}
}

EmberNet::IPeer::IPeer()
{
	myPeer = new CPeer();
}

EmberNet::IPeer::~IPeer()
{
	delete myPeer;
	myPeer = nullptr;
}

bool EmberNet::IPeer::Startup(const EmberNet::SocketDescriptor& aSocketDescriptor,
	const CreateParameters& someCreateParameters) const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	return myPeer->Startup(aSocketDescriptor, someCreateParameters);
}

bool EmberNet::IPeer::Startup(const CreateParameters& someCreateParameters) const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	return myPeer->Startup(someCreateParameters);
}

void EmberNet::IPeer::Connect(const std::string& aRemoteAddres, const unsigned short aPort) const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	myPeer->Connect(aRemoteAddres, aPort);
}

/**
 * \brief 
 * \return Returns true if there is still messages laying in the buffer
 */
bool EmberNet::IPeer::WorkThroughMessages() const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	return myPeer->WorkThroughMessages();
}

void EmberNet::IPeer::Shutdown() const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	myPeer->Shutdown();
}

unsigned EmberNet::IPeer::GetUID() const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	return myPeer->GetUID();
}

void EmberNet::IPeer::Send(const char* someDataToSend, const unsigned aDataSize, EmberNet::SendFlags aFlag) const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	myPeer->Send(someDataToSend, aDataSize, aFlag);
}

void EmberNet::IPeer::MapFunctionToMessage(std::function<void(char*, unsigned int)> aFunction, unsigned char aMessage) const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	myPeer->MapFunctionToMessage(std::move(aFunction), aMessage);
}

EmberNet::NetworkDiagnostics EmberNet::IPeer::GetNetworkDiagnostics() const
{
	EMBER_NET_ASSERT(myPeer != nullptr, "Peer is nullptr. Have you ran the constructor...?");

	return myPeer->GetNetworkDiagnostics();
}
