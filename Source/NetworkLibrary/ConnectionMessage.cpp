#include "ConnectionMessage.h"

#include "Serializer.h"

void EmberNet::CConnectMessage::Serialize(std::vector<char>& someData) const
{
	CNetMessage::Serialize(someData);
}

void EmberNet::CConnectMessage::DeSerialize(std::vector<char>& someData)
{
	CNetMessage::DeSerialize(someData);
}
