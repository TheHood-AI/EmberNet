#include "stdafx.h"
#include "Client.h"

#include "../../EmberNet/MessagesTypes.h"
#include "../TShared/NetMessage.h"
#include "../../EmberNet/Serializer.h"
#include "../../EmberNet/DisconnectMessage.h"
#include <iostream>
#include <fstream>

void CClient::Start()
{
	EmberNet::StartWSA();
	EmberNet::CreateParameters createParameters;
	createParameters.myMillisecondsBetweenPing = 1000;
	if (!myPeer.Startup(createParameters))
	{
		printf("Failed to start peer\n");
	}



	myPeer.MapFunctionToMessage([this](char* someData, unsigned short aSize)
	{
		std::ifstream stream;
		stream.open("BigTGA.tga", std::ios::binary);
		
		if (stream.is_open())
		{
			std::cout << "Hey!" << std::endl;
		}
		stream.ignore(std::numeric_limits<std::streamsize>::max());
		std::streamsize length = stream.gcount();
		stream.clear();   //  Since ignore will have set eof.
		stream.seekg(0, std::ios_base::beg);
		
		char* buffer = new char[length + 1];
		stream.read(&buffer[1], length);
		buffer[0] = CAST_TO_UCHAR(ENetMessageType::Image);
		myPeer.Send(buffer, length + 1, EmberNet::SendFlags::BigPackage);
		delete[] buffer;
		buffer = nullptr;
	}, CAST_TO_UCHAR(EmberNet::ENetMessageType::ConnectionConfirm));
	myPeer.MapFunctionToMessage(BIND(CClient::PlayerConnected, this), CAST_TO_UCHAR(ENetMessageType::PlayerConnected));
	myPeer.MapFunctionToMessage(BIND(CClient::PlayerDisconnected, this), CAST_TO_UCHAR(EmberNet::ENetMessageType::DisconnectMsg));
	myPeer.MapFunctionToMessage(BIND(CClient::CreateObject, this), CAST_TO_UCHAR(ENetMessageType::CreateObject));
	myPeer.MapFunctionToMessage([this](char* someData, unsigned int aSize)
	{
		CSetPlayerPosMessage msg;
		EmberNet::DeSerializeMessage(msg, someData, aSize);
		SNetworkEvent event;
		event.myType = ENetworkEventType::SetPlayerPos;
		event.u_myPlayerPos.myID = msg.myID;
		event.u_myPlayerPos.myPos = msg.myPos;
		myPostMaster->ScheduleEvent(event);
	}, CAST_TO_UCHAR(ENetMessageType::SetPlayerPos));
}

void CClient::CreateObject(char* someData, unsigned int aSize)
{
	CCreateObjectMessage msg;
	EmberNet::DeSerializeMessage(msg, someData, aSize);

	SNetworkEvent event;
	event.myType = ENetworkEventType::CreateObject;
	event.u_myObjectData.myID = msg.GetUID();
	event.u_myObjectData.myPos = msg.myPos;
	myPostMaster->ScheduleEvent(event);
}

void CClient::Connect(const std::string& aConnectionAddres, const unsigned short aPort) const
{
	myPeer.Connect(aConnectionAddres, aPort);
}

void CClient::Update() const
{
	myPeer.WorkThroughMessages();
}

void CClient::Send(const char* someData, const int aDataSize) const
{
	myPeer.Send(someData, aDataSize);
}

int CClient::GetPeerID()
{
	return myPeer.GetUID();
}

void CClient::SetPostMaster(CNetworkPostMaster* aPostMaster)
{
	myPostMaster = aPostMaster;
}

void CClient::ConnectionConfirm(char* someData, unsigned int aSize)
{
	printf("CLIENT: Connection!. Got ID: %d", myPeer.GetUID());
}

void CClient::PlayerConnected(char* someData, unsigned int aSize) const
{
	CPlayerConnectedMessage msg;
	EmberNet::DeSerializeMessage(msg, someData, aSize);
	SNetworkEvent event;
	event.myType = ENetworkEventType::PlayerConnected;
	event.u_myPlayerConnectedData.myID = msg.myID;
	event.u_myPlayerConnectedData.myPos = msg.myPos;
	myPostMaster->ScheduleEvent(event);
}

void CClient::PlayerDisconnected(char* someData, unsigned int aSize) const
{
	EmberNet::CDisConnectMessage msg;
	EmberNet::DeSerializeMessage(msg, someData, aSize);
	SNetworkEvent event;
	event.myType = ENetworkEventType::PlayerDisconnected;
	event.u_myPlayerDisconnectedData.myID = msg.myUID;
	myPostMaster->ScheduleEvent(event);
	printf("Someone disconnected!");
}
