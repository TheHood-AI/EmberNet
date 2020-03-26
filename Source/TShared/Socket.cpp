#include "pch.h"
#include "Socket.h"

CSocket::~CSocket()
{
	closesocket(mySocket);
}

bool CSocket::SocketSetup(const bool aIsBlocking, const unsigned short aPortToUse, const std::string& aIPToRecieveMsgFrom)
{
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (!aIsBlocking)
	{
		u_long ne = TRUE;
		ioctlsocket(mySocket, FIONBIO, &ne);
	}

	if (mySocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError());
		return false;
	}
	
	printf("Socket is ok!\n");

	mySockAddr.sin_family = AF_INET;
	mySockAddr.sin_port = htons(aPortToUse);
	mySockAddr.sin_addr.S_un.S_addr = inet_addr(aIPToRecieveMsgFrom.c_str());


	if (bind(mySocket, reinterpret_cast<SOCKADDR*>(&mySockAddr), sizeof(mySockAddr)) == SOCKET_ERROR)
	{
		printf("Bind failed! Error: %d.\n", WSAGetLastError());
		closesocket(mySocket);
		return false;
	}
	else
		printf("Bind is OK!\n");

	return true;
}

bool CSocket::SocketSetup(const bool aIsBlocking)
{
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	u_long ne = !aIsBlocking;
	ioctlsocket(mySocket, FIONBIO, &ne);

	if (mySocket == INVALID_SOCKET)
	{
		printf("Error at socket() %d\n", WSAGetLastError());
		return false;
	}

	printf("Socket is ok!\n");
	return true;
}

void CSocket::SetRecieveBufferSize(const unsigned aBufferSize) const
{
	setsockopt(mySocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char const*>(&aBufferSize), sizeof(aBufferSize));
}

void CSocket::SetSendBufferSize(const unsigned aBufferSize) const
{
	setsockopt(mySocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char const*>(&aBufferSize), sizeof(aBufferSize));
}

const SOCKET& CSocket::GetSocket() const
{
	return mySocket;
}
