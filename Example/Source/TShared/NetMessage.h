#pragma once
#include <vector>
#include <string>
#include "Vector2.hpp"
#include "MessagesTypes.h"
#include "NetMessage.h"
#include "../../../EmberNet/NetMessage.h"

enum class ENetMessageType : char
{
	PlayerConnected = EmberNet::GetUserEnumStart(),
	CreateObject,
	BigPackage,
	BigPackageChunk,
	ReadyForChunk,
	Image,
	SetPlayerPos
};

class CPlayerConnectedMessage : public EmberNet::CNetMessage
{
public:
	CPlayerConnectedMessage() { myMessageType = CAST_TO_UCHAR(ENetMessageType::PlayerConnected); };
	void Serialize(std::vector<char>& someData) const override;
	void DeSerialize(std::vector<char>& someData) override;

	VECTOR2F myPos;
	unsigned int myID;
};

class CSetPlayerPosMessage: public EmberNet::CNetMessage
{
public:
	CSetPlayerPosMessage() { myMessageType = CAST_TO_UCHAR(ENetMessageType::SetPlayerPos); };
	void Serialize(std::vector<char>& someData) const override;
	void DeSerialize(std::vector<char>& someData) override;

	VECTOR2F myPos;
	unsigned int myID;
};

class CCreateObjectMessage : public EmberNet::CNetMessage
{
public:
	CCreateObjectMessage () { myMessageType = CAST_TO_UCHAR(ENetMessageType::CreateObject); };
	void Serialize(std::vector<char>& someData) const override;
	void DeSerialize(std::vector<char>& someData) override;

	VECTOR2F myPos;
	unsigned int myID = 0;
};
