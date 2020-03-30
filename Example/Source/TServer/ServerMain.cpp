#include "pch.h"
#include "ServerMain.h"

#include "../../../EmberNet/Serializer.h"
#include "../TShared/NetMessage.h"
#include "../../../EmberNet/ConnectionMessage.h"
#include "../../../EmberNet/DisconnectMessage.h"

#include "NetworkPostMaster.h"
#include "../TShared/Random.hpp"
#include "Macros.h"
#include <fstream>

CServerMain::CServerMain()
{
}

CServerMain::~CServerMain()
= default;

unsigned short playerUID = 1;
void CServerMain::Start()
{
	EmberNet::StartWSA();

	EmberNet::SocketDescriptor desc;
	desc.myPort = 5150;
	desc.myIPToRecieveFrom = LISTEN_TO_ALL_IP;
	if (!myPeer.Startup(desc, {}))
	{
		printf("Failed to start peer\n");
	}

	myPeer.MapFunctionToMessage(BIND(CServerMain::PlayerConnect, this), CAST_TO_UCHAR(EmberNet::ENetMessageType::ConnectMsg));
	myPeer.MapFunctionToMessage([this](char* someData, unsigned int aSize)
	{
		myPeer.Send(someData, aSize);

		EmberNet::CDisConnectMessage msg;
		EmberNet::DeSerializeMessage(msg, someData, aSize);
		SNetworkEvent event;
		event.myType = ENetworkEventType::PlayerDisconnected;
		event.u_myPlayerDisconnectedData.myID = msg.myUID;
		myPostMaster->ScheduleEvent(event);

		for (int i = myGameData.size(); i > 0; --i)
		{
			if (myGameData[i - 1].myID == msg.myUID)
			{
				CYCLIC_ERASE(myGameData, i - 1);
				break;
			}

		}

	}, CAST_TO_UCHAR(EmberNet::ENetMessageType::DisconnectMsg));
	myPeer.MapFunctionToMessage([](char* someData, unsigned int aSize)
	{
		std::ofstream stream;
		stream.open("BigTGA - copy.tga", std::ios::binary);
		for (int i = 1; i < aSize; ++i)
		{
			stream << someData[i];
		}
		printf("Copied Image!\n");
	}, CAST_TO_UCHAR(ENetMessageType::Image));
	myPeer.MapFunctionToMessage([this](char* someData, unsigned int aSize)
	{
		CPlayerConnectedMessage msg;
		EmberNet::DeSerializeMessage(msg, someData, aSize);

		SNetworkEvent event;
		event.myType = ENetworkEventType::SetPlayerPos;
		event.u_myPlayerPos.myID = msg.myID;
		event.u_myPlayerPos.myPos = msg.myPos;
		myPostMaster->ScheduleEvent(event);

		myPeer.SendToAllExcept(someData, aSize, EmberNet::SendFlags::NoFlag, msg.myID);
	}, CAST_TO_UCHAR(ENetMessageType::SetPlayerPos));

	printf("SERVE: Start\n");
}
void CServerMain::Update()
{
	myTimer.Update();
	myCountTime += myTimer.GetDeltaTime();
	if (myCountTime > 1)
	{
		EmberNet::NetworkDiagnostics diag = myPeer.GetNetworkDiagnostics();
		printf("Send Bytes Last Second: %d\n", diag.mySentbytesLastSecond);
		printf("Recieved Bytes Last Second: %d\n", diag.myRecievedBytesLastSecond);
		printf("Package Drop: %d\n", diag.myPackageDrop);
		for (auto lat : diag.myLatencyInMilliSeconds)
		{
			printf("Latency (UID: %d) : %f ms\n", lat.myPeerID, lat.myMilliSeconds);
		}
	
		myCountTime = 0;
	}
	while(myPeer.WorkThroughMessages());
}

void CServerMain::SetPostMaster(CNetworkPostMaster * aPostMaster)
{
	myPostMaster = aPostMaster;
	myPostMaster->RegisterListener(ENetworkEventType::CreateObject, *this);
}

void CServerMain::PlayerConnect(char* someData, unsigned int aSIze)
{
	EmberNet::CConnectMessage msg;

	EmberNet::DeSerializeMessage(msg, someData, aSIze);


	CPlayerConnectedMessage pMsg;
	pMsg.myID = playerUID++;
	pMsg.myPos = { CU::RandomFloat(0,1), CU::RandomFloat(0,1) };

	auto data = EmberNet::SerializeMessage(pMsg);

	myPeer.Send(data.data(), data.size());
	printf("SERVER: Connection!\n");

	SNetworkEvent event;
	event.myType = ENetworkEventType::PlayerConnected;
	event.u_myPlayerConnectedData.myID = pMsg.myID;
	event.u_myPlayerConnectedData.myPos = pMsg.myPos;
	myPostMaster->ScheduleEvent(event);

	for (auto& gData : myGameData)
	{
		auto pack = EmberNet::SerializeMessage(gData);

		myPeer.Send(pack.data(), pack.size());
	}
	myGameData.push_back(pMsg);
}

void CServerMain::RecieveMessage(const SNetworkEvent * aEvent)
{
	if (aEvent->myType == ENetworkEventType::CreateObject)
	{
		CCreateObjectMessage msg;
		msg.myID = aEvent->u_myObjectData.myID;
		msg.myPos = aEvent->u_myObjectData.myPos;
		auto data = EmberNet::SerializeMessage(msg);

		myPeer.Send(data.data(), data.size(), EmberNet::SendFlags::Guaranteed);
	}
}
