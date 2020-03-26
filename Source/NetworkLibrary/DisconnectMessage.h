#pragma once
#include "NetMessage.h"

namespace EmberNet
{
	class EXPORT CDisConnectMessage : public EmberNet::CNetMessage
	{
	public:
		CDisConnectMessage() { myMessageType = CAST_TO_UCHAR(EmberNet::ENetMessageType::DisconnectMsg); };
		void Serialize(std::vector<char>& someData) const override;
		void DeSerialize(std::vector<char>& someData) override;

		unsigned int myUID = 0;
	};
}