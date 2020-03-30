#pragma once
#include "CommonNetworkIncludes.h"

class CSocket
{
public:
	CSocket() = default;
	~CSocket();
	CSocket(const CSocket& aSocket) = delete;

	
	bool SocketSetup(bool aIsBlocking, unsigned short aPortToUse, const std::string& aIPToRecieveMsgFrom);
	bool SocketSetup(bool aIsBlocking);
	
	void SetRecieveBufferSize(unsigned aBufferSize) const;
	void SetSendBufferSize(unsigned  aBufferSize) const;

	[[nodiscard]] const SOCKET& GetSocket()const;
private:
	SOCKET mySocket{};
	SOCKADDR_IN mySockAddr{};
};
