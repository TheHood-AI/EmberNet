#include "pch.h"
#include "DisconnectMessage.h"

#include "Serializer.h"

void EmberNet::CDisConnectMessage::Serialize(std::vector<char>& someData) const
{
	CNetMessage::Serialize(someData);

	SerializeType(myUID, someData);
}

void EmberNet::CDisConnectMessage::DeSerialize(std::vector<char>& someData)
{
	DeSerializeType(myUID, someData);

	CNetMessage::DeSerialize(someData);
}
