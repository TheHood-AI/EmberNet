#pragma once
#include "CommonNetworkIncludes.h"
#include <string>

#define ANY_PORT 0
namespace EmberNet
{
	class CSocket
	{
	public:
		CSocket() = default;
		~CSocket();
		CSocket(const CSocket& aSocket) = delete;


		bool SocketSetup();
		bool SocketSetup(const unsigned short aPortToUse, const std::string& aIPToListenTo);

		void SetRecieveBufferSize(unsigned aBufferSize) const;
		void SetSendBufferSize(unsigned  aBufferSize) const;

		[[nodiscard]] const SOCKET& GetSocket()const;
	private:
		SOCKET mySocket;
	};
}
