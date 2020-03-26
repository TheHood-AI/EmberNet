#include "Socket.h"
#include "HelperMacros.h"

namespace EmberNet
{
	CSocket::~CSocket()
	{
		closesocket(mySocket);
	}

	bool CSocket::SocketSetup()
	{
		mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		u_long ne = TRUE;
		ioctlsocket(mySocket, FIONBIO, &ne);


		if (mySocket == INVALID_SOCKET)
		{
			EMBER_NET_LOG("Failed to create socket! WSA error code: %d\n", WSAGetLastError());
			return false;
		}


		return true;
	}

	bool CSocket::SocketSetup(const unsigned short aPortToUse, const std::string& aIPToListenTo)
	{
		SocketSetup();

		SOCKADDR_IN addr;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(aPortToUse);
		addr.sin_addr.S_un.S_addr = inet_addr(aIPToListenTo.c_str());

		if (bind(mySocket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR)
		{
			EMBER_NET_LOG("Failed to create socket! WSA error code: %d.\n", WSAGetLastError());
			closesocket(mySocket);
			return false;
		}

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
}