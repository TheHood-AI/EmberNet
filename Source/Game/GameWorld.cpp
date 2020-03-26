#include "stdafx.h"
#include "GameWorld.h"
#include "tga2d/sprite/sprite.h"
#include <thread>
#include "C_Character.h"
#include "Macros.h"

#include "Game.h"
#include "C_Object.h"
#include "NetMessage.h"
#include "../NetworkLibrary/Serializer.h"

C_GameWorld::C_GameWorld()
{
}

C_GameWorld::~C_GameWorld()
= default;



void C_GameWorld::Init()
{
	myClient.Start();
	myClient.Connect("151.177.31.216", 5150);

	myClient.SetPostMaster(&myPostMaster);
	myPostMaster.RegisterListener(ENetworkEventType::PlayerConnected, *this);
	myPostMaster.RegisterListener(ENetworkEventType::PlayerDisconnected, *this);
	myPostMaster.RegisterListener(ENetworkEventType::CreateObject, *this);
	myPostMaster.RegisterListener(ENetworkEventType::SetPlayerPos, *this);
	myPostMaster.RegisterListener(ENetworkEventType::SendPlayerPos, *this);

	std::thread clientUpdateThread([this]()
	{
		while (!CGame::ShouldQuit)
		{
			myClient.Update();
			Sleep(1000 / 60000);
		}
	});
	clientUpdateThread.detach();


}

void C_GameWorld::Update(float aTimeDelta)
{
	Sh_GameWorld::Update(aTimeDelta);

	for(auto& obj : myObjects)
	{
		obj->Update(aTimeDelta);
	}

	myPostMaster.DispatchScheduledEvents();
}

void C_GameWorld::RecieveMessage(const SNetworkEvent * aEvent)
{
	switch (aEvent->myType)
	{
	case  ENetworkEventType::PlayerConnected:
	{
		bool canAdd = true;
		for (auto& obj : myGameObjects)
		{
			if (obj->GetID() == aEvent->u_myPlayerConnectedData.myID)
			{
				canAdd = false;
				break;
			}
		}
		if (canAdd)
		{
			auto ch = new C_Character();
			ch->Init();
			ch->SetPosition(aEvent->u_myPlayerConnectedData.myPos);
			ch->SetID(aEvent->u_myPlayerConnectedData.myID);
			ch->myPostMaster = &myPostMaster;
			ch->myPeerID = myClient.GetPeerID();
			myGameObjects.push_back(ch);
		}
		break;
	}
	case ENetworkEventType::PlayerDisconnected:
	{
		for (int i = myGameObjects.size(); i > 0; --i)
		{
			if (myGameObjects[i - 1]->GetID() == aEvent->u_myPlayerDisconnectedData.myID)
			{
				delete myGameObjects[i - 1];
				myGameObjects[i - 1] = nullptr;
				CYCLIC_ERASE(myGameObjects, i - 1);
				break;
			}
		}

		break;
	}
	case ENetworkEventType::CreateObject:
	{
		auto obj = new C_Object(aEvent->u_myObjectData.myPos, &myPostMaster);
		obj->Init();
		obj->SetID(aEvent->u_myObjectData.myID);
		myObjects.push_back(obj);
			break;
	}
	case ENetworkEventType::SendPlayerPos:
		{
			CSetPlayerPosMessage msg;
			msg.myID = aEvent->u_myPlayerPos.myID;
			msg.myPos = aEvent->u_myPlayerPos.myPos;
			auto data = EmberNet::SerializeMessage(msg);
			myClient.Send(data.data(), data.size());
			break;
		}
	case ENetworkEventType::SetPlayerPos:
	{
		for(auto& obj : myGameObjects)
		{
			if(obj->GetID() == aEvent->u_myPlayerPos.myID)
			{
				obj->SetPosition(aEvent->u_myPlayerPos.myPos);
			}
		}
	}
	default:
		break;;
	}
}
