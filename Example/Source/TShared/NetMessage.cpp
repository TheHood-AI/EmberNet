#include "pch.h"
#include "NetMessage.h"
#include "NetworkHelpers.h"
#include "NetworkPostMaster.h"
#include "Serializer.h"

static unsigned short globalNetMessageUIDCounter = 0;

void CPlayerConnectedMessage::Serialize(std::vector<char> & someData) const
{
	CNetMessage::Serialize(someData);

	CNetworkHelpers::SerializeType(myPos.x, someData);
	CNetworkHelpers::SerializeType(myPos.y, someData);

	CNetworkHelpers::SerializeType(myID, someData);
	EmberNet::SerializeType(myPos.x, someData);
}

void CPlayerConnectedMessage::DeSerialize(std::vector<char> & someData)
{
	CNetworkHelpers::DeSerializeType(myID, someData);

	CNetworkHelpers::DeSerializeType(myPos.y, someData);
	CNetworkHelpers::DeSerializeType(myPos.x, someData);

	CNetMessage::DeSerialize(someData);
}

void CSetPlayerPosMessage::Serialize(std::vector<char>& someData) const
{
	CNetMessage::Serialize(someData);

	CNetworkHelpers::SerializeType(myPos.x, someData);
	CNetworkHelpers::SerializeType(myPos.y, someData);

	CNetworkHelpers::SerializeType(myID, someData);
}

void CSetPlayerPosMessage::DeSerialize(std::vector<char>& someData)
{
	CNetworkHelpers::DeSerializeType(myID, someData);

	CNetworkHelpers::DeSerializeType(myPos.y, someData);
	CNetworkHelpers::DeSerializeType(myPos.x, someData);

	CNetMessage::DeSerialize(someData);

}

void CCreateObjectMessage::Serialize(std::vector<char>& someData) const
{
	CNetMessage::Serialize(someData);

	CNetworkHelpers::SerializeType(myPos.x, someData);
	CNetworkHelpers::SerializeType(myPos.y, someData);

	CNetworkHelpers::SerializeType(myID, someData);
}

void CCreateObjectMessage::DeSerialize(std::vector<char>& someData)
{
	CNetworkHelpers::DeSerializeType(myID, someData);

	CNetworkHelpers::DeSerializeType(myPos.y, someData);
	CNetworkHelpers::DeSerializeType(myPos.x, someData);

	CNetMessage::DeSerialize(someData);
}
