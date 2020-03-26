#include "pch.h"
#include "NetworkHelpers.h"

#include "Macros.h"
#include "NetworkPostMaster.h"

#include <iostream>

CNetworkHelpers* CNetworkHelpers::ourInstance = nullptr;

void CNetworkHelpers::Update(const float aDelta)
{
	std::lock_guard<std::mutex> lock(myMutex);
	for (int i = gsl::narrow<int>(myGuaranteedMessages.size()) - 1; i >= 0; --i)
	{
		auto& msg = myGuaranteedMessages[i];
		msg.myTimeoutTimer -= aDelta;
		if (msg.myTimeoutTimer < 0)
		{
			msg.myResendTries++;
			msg.myCallback();
			msg.myTimeoutTimer = 0.2f;
		}
	}
}

CNetworkHelpers* CNetworkHelpers::GetInstance()
{
	if (ourInstance == nullptr)
		ourInstance = new CNetworkHelpers();

	return ourInstance;
}

void CNetworkHelpers::ConvertMessageAndDoWork(const char* aBuffer, const int aByteRecieved, CNetworkPostMaster* aEventManager)
{
	switch (aBuffer[0])
	{
		case static_cast<char>(ENetMessageType::PlayerConnected) :
		{
			break;
		}
		default:
			std::cout << "Unhandled msg recieved!!!" << aBuffer[0] << std::endl;
			break;
	}
}

void CNetworkHelpers::Acknowledge(const unsigned short aUID)
{
	for (int i = static_cast<int>(myGuaranteedMessages.size()) - 1; i >= 0; --i)
	{
		if (myGuaranteedMessages[i].myMessageUID == aUID)
		{
			std::lock_guard<std::mutex> lock(myMutex);
			CYCLIC_ERASE(myGuaranteedMessages, i);
			break;
		}
	}
}

int CNetworkHelpers::RecieveMsg(const CSocket& aSocket, char* aRecieveBuffer, SOCKADDR_IN& aSenderAddr)
{
	int senderSize = sizeof(aSenderAddr);

	const int size = recvfrom(aSocket.GetSocket(), aRecieveBuffer, 512, 0, reinterpret_cast<SOCKADDR*>(&aSenderAddr), &senderSize);
	if(size > 0)
		myRecievedBytes += size;
	return size;
}

bool CNetworkHelpers::WSASetup()
{
	WSADATA wsaData;

	const WORD wVersionRequested = MAKEWORD(2, 2);

	const int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		printf("WSAStartup failed with error: %d\n", err);
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		printf("Could not find a usable version of Windsock.dll\n");
		WSACleanup();
		return false;
	}
	else
		printf("The windock 2.2 dll was found okay\n");

	return true;
}

void CNetworkHelpers::SendMsg(const char* aMsg, const int aMessageSize, const CSocket& aSenderSocket, const SOCKADDR_IN& aRecieverAddres)
{
	mySentBytes += gsl::narrow<int>(aMessageSize);
	
	sendto(aSenderSocket.GetSocket(), aMsg, aMessageSize, 0,
		reinterpret_cast<const SOCKADDR*>(&aRecieverAddres), sizeof(aRecieverAddres));
}