#pragma once

enum class ENetworkEventType
{
	PlayerConnected,
	PlayerDisconnected,
	CreateObject,
	SetPlayerPos,
	SendPlayerPos,
	COUNT
};

struct SPlayerDisConnectedData
{
	unsigned short myID = 0;
};

struct SPlayerConnectedData
{
	unsigned short myID = 0;
	VECTOR2F myPos;
};

struct SCreateObject
{
	unsigned short myID = 0;
	VECTOR2F myPos;
};

struct SSetPlayerPos
{
	unsigned short myID = 0;
	VECTOR2F myPos;
};