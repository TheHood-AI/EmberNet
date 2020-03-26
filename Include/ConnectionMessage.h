#pragma once
#include "NetMessage.h"

namespace EmberNet
{
	class EXPORT CConnectMessage : public EmberNet::CNetMessage
	{
	public:
		CConnectMessage() { myMessageType = CAST_TO_UCHAR(ENetMessageType::ConnectMsg); };
		void Serialize(std::vector<char>& someData) const override;
		void DeSerialize(std::vector<char>& someData) override;
	};
}